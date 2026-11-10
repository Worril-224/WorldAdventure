# WAUI 边界

拥有 `Source/WAUI` 和 `Content/WA/UI`。实现主菜单、HUD、玩家列表和连接提示。仅依赖 WACore、WAGame；玩家生命通过 `IWAPlayerInfo` 事件读取。发送 Host/Join/交互意图，但不得直接改生命、伤害、模式、复活或复制状态。不得被其他运行时模块反向依赖。
