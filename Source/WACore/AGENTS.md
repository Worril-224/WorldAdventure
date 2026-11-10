# WACore 边界

拥有 `Source/WACore` 和 `Content/WA/Core`。只放跨模块公共枚举/结构、日志、Gameplay Tags、只读玩家信息接口、交互接口和游戏规则接口。不得 include 其他 WA 模块，不得放具体角色、伤害、地图或 UI 实现。公共类型变更由集成负责人审核并同步 `Docs/ModuleContracts.md`。
