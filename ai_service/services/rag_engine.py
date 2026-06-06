"""
RAG 引擎（LangChain 版）
参考来源：
- 02_LangChain_3_文本切分.py（RecursiveCharacterTextSplitter）
- 04-government-rag（RAG 架构）
"""
import os
import pickle
from typing import List, Dict, Any
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_community.vectorstores import FAISS
from langchain_community.embeddings import HuggingFaceEmbeddings
from langchain_core.prompts import ChatPromptTemplate
from langchain.chains.combine_documents import create_stuff_documents_chain
from core.config import settings
from core.llm_provider import get_llm


class RAGEngine:
    """基于 LangChain 的 RAG 引擎"""

    def __init__(self):
        cfg = settings.rag
        self.enabled = cfg.get("enabled", True)
        self.chunk_size = cfg.get("chunk_size", 512)
        self.chunk_overlap = cfg.get("chunk_overlap", 64)
        self.top_k = cfg.get("top_k", 5)
        self.kb_dir = cfg.get("knowledge_base_dir", "./knowledge_base")
        self.index_path = os.path.join(self.kb_dir, "faiss.index")
        self.docs_path = os.path.join(self.kb_dir, "docs.pkl")

        # LangChain RecursiveCharacterTextSplitter（从你学习代码中提炼）
        self.text_splitter = RecursiveCharacterTextSplitter(
            chunk_size=self.chunk_size,
            chunk_overlap=self.chunk_overlap,
            length_function=len,
            is_separator_regex=False,
        )

        # Embedding：本地 HuggingFace 模型（BAAI/bge-small-zh-v1.5）
        model_name = cfg.get("embedding_model", "BAAI/bge-small-zh-v1.5")
        self.embeddings = HuggingFaceEmbeddings(
            model_name=model_name,
            model_kwargs={"device": "cpu"},
            encode_kwargs={"normalize_embeddings": True}
        )

        self.vectorstore = None
        self._load_index()

    def _load_index(self):
        """加载已有索引"""
        try:
            if os.path.exists(self.index_path) and os.path.exists(self.docs_path):
                self.vectorstore = FAISS.load_local(
                    self.kb_dir, self.embeddings, allow_dangerous_deserialization=True
                )
                print(f"[RAG] 加载索引成功")
            else:
                print("[RAG] 未找到已有索引，等待文档上传")
        except Exception as e:
            print(f"[RAG] 加载索引失败: {e}")
            self.enabled = False

    def _save_index(self):
        """保存索引"""
        if self.vectorstore is None:
            return
        os.makedirs(self.kb_dir, exist_ok=True)
        self.vectorstore.save_local(self.kb_dir)

    def add_document(self, text: str, metadata: Dict[str, Any] = None) -> bool:
        """添加文档到知识库"""
        if not self.enabled:
            return False

        try:
            docs = self.text_splitter.create_documents(
                texts=[text],
                metadatas=[metadata or {}]
            )

            if self.vectorstore is None:
                self.vectorstore = FAISS.from_documents(docs, self.embeddings)
            else:
                self.vectorstore.add_documents(docs)

            self._save_index()
            print(f"[RAG] 添加文档成功，共 {len(docs)} 个片段")
            return True
        except Exception as e:
            print(f"[RAG] 添加文档失败: {e}")
            return False

    def query(self, query: str, top_k: int = None) -> Dict[str, Any]:
        """RAG 问答：检索 → LangChain Chain → 生成答案"""
        if not self.enabled or self.vectorstore is None:
            return {"answer": "知识库为空，请先上传文档。", "sources": []}

        k = top_k or self.top_k

        try:
            # 1. 检索相关文档
            retriever = self.vectorstore.as_retriever(search_kwargs={"k": k})
            docs = retriever.invoke(query)

            if not docs:
                return {"answer": "未找到相关资料。", "sources": []}

            # 2. 构建 LangChain RAG Chain
            prompt = ChatPromptTemplate.from_messages([
                ("system", "你是医院运营管理专家，请结合给定资料回答用户问题。\n\n{context}"),
                ("human", "{input}")
            ])

            combine_docs_chain = create_stuff_documents_chain(get_llm(), prompt)
            answer = combine_docs_chain.invoke({
                "context": docs,
                "input": query
            })

            return {
                "answer": answer,
                "sources": [
                    {"content": d.page_content, "metadata": d.metadata}
                    for d in docs
                ]
            }
        except Exception as e:
            return {"answer": f"RAG 查询失败: {str(e)}", "sources": []}


rag_engine = RAGEngine()
