"""
LLM 客户端（LangChain 版）
参考来源：02_LangChain_1_调用模型.py
"""
from langchain_core.messages import SystemMessage, HumanMessage
from core.llm_provider import get_llm
from core.exceptions import LLMException


class LLMClient:
    """基于 LangChain 的大语言模型客户端"""

    def __init__(self):
        self.model = get_llm()

    def chat(self, system_prompt: str, user_prompt: str, temperature: float = None) -> str:
        """同步对话：SystemMessage + HumanMessage → invoke"""
        try:
            model = get_llm(temperature) if temperature is not None else self.model
            messages = [
                SystemMessage(content=system_prompt),
                HumanMessage(content=user_prompt)
            ]
            return model.invoke(messages).content
        except Exception as e:
            raise LLMException(f"LLM 调用失败: {str(e)}")

    def invoke_with_prompt(self, prompt_value, temperature: float = None) -> str:
        """直接 invoke ChatPromptValue（用于 ChatPromptTemplate）"""
        try:
            model = get_llm(temperature) if temperature is not None else self.model
            return model.invoke(prompt_value).content
        except Exception as e:
            raise LLMException(f"LLM 调用失败: {str(e)}")


llm_client = LLMClient()
