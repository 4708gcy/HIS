"""RAG 知识库路由"""
import os
import time
import uuid
from fastapi import APIRouter, Depends, UploadFile, File, Form, BackgroundTasks
from core.security import verify_api_key
from core.response import ResponseBuilder
from schemas import RAGChatRequest, DocumentUploadResponse
from services.rag_engine import rag_engine

router = APIRouter(prefix="/api/v2/rag", tags=["知识库"])


@router.post("/chat", dependencies=[Depends(verify_api_key)])
def rag_chat(req: RAGChatRequest):
    """RAG 知识库问答"""
    start = time.time()
    result = rag_engine.query(req.query, top_k=req.top_k)
    return ResponseBuilder.success(result, "问答完成", start_time=start)


@router.post("/upload", dependencies=[Depends(verify_api_key)])
async def upload_document(
    background_tasks: BackgroundTasks,
    file: UploadFile = File(...),
    title: str = Form(None)
):
    """上传文档到知识库（后台异步处理）"""
    start = time.time()
    doc_id = str(uuid.uuid4())[:8]

    kb_dir = "./knowledge_base/files"
    os.makedirs(kb_dir, exist_ok=True)
    file_path = os.path.join(kb_dir, f"{doc_id}_{file.filename}")

    with open(file_path, "wb") as f:
        content = await file.read()
        f.write(content)

    background_tasks.add_task(_process_document, file_path, doc_id, title or file.filename)

    return ResponseBuilder.success({
        "document_id": doc_id,
        "filename": file.filename,
        "status": "processing"
    }, "文档上传成功，后台处理中", start_time=start)


def _process_document(file_path, doc_id, title):
    if file_path.endswith(".pdf"):
        try:
            import pdfplumber
        except ImportError:
            print(f"[RAG] pdfplumber 未安装，跳过 PDF: {file_path}")
            return
        with pdfplumber.open(file_path) as pdf:
            text = "".join(page.extract_text() or "" for page in pdf.pages)
    elif file_path.endswith(".txt"):
        with open(file_path, "r", encoding="utf-8") as f:
            text = f.read()
    else:
        print(f"[RAG] 不支持的文件格式: {file_path}")
        return

    if text:
        rag_engine.add_document(text, metadata={"doc_id": doc_id, "title": title})
        print(f"[RAG] 文档处理完成: {title}")
