"""配置加载"""
import os
import yaml


class Settings:
    """配置管理，env 覆盖 config.yaml"""
    pass

    def __init__(self):
        self._load()

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

    def section(self, name: str) -> dict:
        return self._config.get(name, {})

    @property
    def llm(self) -> dict:
        return self.section("llm")

    @property
    def database(self) -> dict:
        return self.section("database")

    @property
    def security(self) -> dict:
        return self.section("security")

    @property
    def rag(self) -> dict:
        return self.section("rag")

    @property
    def analysis(self) -> dict:
        return self.section("analysis")


settings = Settings()
