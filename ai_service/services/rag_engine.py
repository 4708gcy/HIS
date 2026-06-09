"""RAG 引擎"""
import os
import pickle
from typing import List, Dict, Any
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_community.vectorstores import FAISS
from langchain_huggingface import HuggingFaceEmbeddings
from langchain_core.prompts import ChatPromptTemplate
from langchain_core.output_parsers import StrOutputParser
from core.config import settings
from core.llm_provider import get_llm


class RAGEngine:

    def __init__(self):
        cfg = settings.rag
        self.enabled = cfg.get("enabled", True)
        self.chunk_size = cfg.get("chunk_size", 512)
        self.chunk_overlap = cfg.get("chunk_overlap", 64)
        self.top_k = cfg.get("top_k", 5)
        self.kb_dir = cfg.get("knowledge_base_dir", "./knowledge_base")
        self.index_path = os.path.join(self.kb_dir, "index.faiss")
        self.docs_path = os.path.join(self.kb_dir, "index.pkl")

        # 滑动窗口分块
        self.text_splitter = RecursiveCharacterTextSplitter(
            chunk_size=self.chunk_size,
            chunk_overlap=self.chunk_overlap,
            length_function=len,
            is_separator_regex=False,
        )

        # bge-small-zh-v1.5（优先使用本地路径）
        model_name = cfg.get("embedding_model", "BAAI/bge-small-zh-v1.5")
        local_model = cfg.get("embedding_model_local", "")
        if local_model and os.path.isdir(local_model):
            model_name = local_model
        self.embeddings = HuggingFaceEmbeddings(
            model_name=model_name,
            model_kwargs={"device": "cpu"},
            encode_kwargs={"normalize_embeddings": True}
        )

        self.vectorstore = None
        self._load_index()

    def _load_index(self):
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
        if self.vectorstore is None:
            return
        os.makedirs(self.kb_dir, exist_ok=True)
        self.vectorstore.save_local(self.kb_dir)

    def add_document(self, text, metadata=None):
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

    def query(self, query, top_k=None):
        if not self.enabled or self.vectorstore is None:
            return {"answer": "知识库为空，请先上传文档。", "sources": []}

        k = top_k or self.top_k

        try:
            # 检索
            retriever = self.vectorstore.as_retriever(search_kwargs={"k": k})
            docs = retriever.invoke(query)

            if not docs:
                return {"answer": "未找到相关资料。", "sources": []}

            # 构建 chain（LCEL 方式）
            prompt = ChatPromptTemplate.from_messages([
                ("system", "你是医院运营管理专家，请结合给定资料回答用户问题。\n\n{context}"),
                ("human", "{input}")
            ])

            chain = prompt | get_llm() | StrOutputParser()
            context_text = "\n\n".join([d.page_content for d in docs])
            answer = chain.invoke({
                "context": context_text,
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
