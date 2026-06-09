"""
自定义异常
"""


class HISException(Exception):
    def __init__(self, msg: str, code: int = 500):
        self.msg = msg
        self.code = code
        super().__init__(msg)


class LLMException(HISException):
    """LLM 调用异常"""
    def __init__(self, msg: str):
        super().__init__(msg, code=503)


