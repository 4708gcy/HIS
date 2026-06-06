"""
配置加载模块
参考来源：04-government-rag 的 yaml.safe_load 配置驱动
"""
import os
import yaml
from typing import Dict, Any


class Settings:
    """配置单例，支持环境变量覆盖 YAML 配置"""
    _instance = None
    _config: Dict[str, Any] = {}

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._load()
        return cls._instance

    def _load(self):
        config_path = os.path.join(os.path.dirname(__file__), "..", "config.yaml")
        with open(config_path, "r", encoding="utf-8") as f:
            self._config = yaml.safe_load(f)

        # 环境变量覆盖
        self._override_from_env("llm", "api_key", "HIS_LLM_API_KEY")
        self._override_from_env("llm", "base_url", "HIS_LLM_BASE_URL")
        self._override_from_env("security", "api_key", "HIS_API_KEY")
        self._override_from_env("database", "host", "HIS_DB_HOST")
        self._override_from_env("database", "port", "HIS_DB_PORT", int)
        self._override_from_env("database", "password", "HIS_DB_PASSWORD")

    def _override_from_env(self, section: str, key: str, env_var: str, cast=None):
        val = os.environ.get(env_var)
        if val is not None:
            if cast:
                val = cast(val)
            self._config.setdefault(section, {})[key] = val

    def get(self, section: str, key: str, default=None):
        return self._config.get(section, {}).get(key, default)

    def section(self, name: str) -> Dict[str, Any]:
        return self._config.get(name, {})

    @property
    def llm(self) -> Dict[str, Any]:
        return self.section("llm")

    @property
    def database(self) -> Dict[str, Any]:
        return self.section("database")

    @property
    def security(self) -> Dict[str, Any]:
        return self.section("security")

    @property
    def rag(self) -> Dict[str, Any]:
        return self.section("rag")

    @property
    def analysis(self) -> Dict[str, Any]:
        return self.section("analysis")


settings = Settings()
