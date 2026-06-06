"""LLM 客户端"""
from langchain_core.messages import SystemMessage, HumanMessage
from core.llm_provider import get_llm
from core.exceptions import LLMException


class LLMClient:

    def __init__(self):
        self.model = get_llm()

    def _invoke(self, prompt_value, temperature=None):
        model = get_llm(temperature) if temperature is not None else self.model
        try:
            return model.invoke(prompt_value).content
        except Exception as e:
            raise LLMException(f"LLM 调用失败: {e}")

    def chat(self, system_prompt, user_prompt, temperature=None):
        messages = [
            SystemMessage(content=system_prompt),
            HumanMessage(content=user_prompt)
        ]
        return self._invoke(messages, temperature)

    def invoke_with_prompt(self, prompt_value, temperature=None):
        return self._invoke(prompt_value, temperature)


llm_client = LLMClient()
