# WorldAdventure 总规范

## 基线

- 引擎：UE 5.8.4，所有机器使用相同补丁版本。项目名为 `WorldAdventure`，代码/资产内部前缀为 `WA`。
- 目标：Windows、2–4 人局域网、Listen Server、第三人称、World Partition、GAS、Enhanced Input。
- 核心规则和网络接口使用 C++；蓝图只保存数据、资产引用和表现逻辑。正式美术、地图、技能和特效不属于当前 Demo。
- 构建：运行 `Scripts/BuildEditor.bat`；打包：运行 `Scripts/PackageDemo.bat`。引擎自带 .NET 10，不要误用系统 .NET 6。

## 所有权与依赖

- `WorldAdventure`：仅提供主游戏模块和启动符号，不实现玩法。
- `WACore`：公共类型、接口、日志和 Native Gameplay Tags，不依赖任何业务模块。
- `WAAbilities -> WACore`：GAS、生命、测试攻击和死亡通知，不引用具体玩家或 UI。
- `WAWorld -> WACore`：World Partition 支持、探索点和世界交互。
- `WAPlayer -> WACore, WAAbilities`：Character、Controller、PlayerState、输入、相机和占位表现。
- `WAGame -> WACore, WAAbilities, WAWorld, WAPlayer`：组合根、会话、GameMode/GameState、合作/PvP 和复活。
- `WAUI -> WACore, WAGame`：菜单与 HUD，只通过接口读取玩家信息，不修改游戏规则。
- `WAEditor`：只读依赖运行时模块；任何运行时模块不得依赖它。
- 禁止循环依赖、跨层include私有头、在 Core 堆积业务、在 Level Blueprint/UI 写游戏规则。

## 修改规则

- 先读本文件和任务涉及模块的 `AGENTS.md`。只修改任务拥有路径；共享文件由集成负责人修改。
- 共享文件：`.uproject`、Target、`Config/*.ini`、`Content/WA/Maps`、公共契约文档。其他 Agent 需要修改时先提出契约变更。
- 公共 API、复制属性、RPC、Gameplay Tag 或默认资产路径变化时，同步更新 `Docs/ModuleContracts.md`、`Docs/Networking.md` 和所有调用方。
- 地图使用 World Partition/OFPA。不要手工重命名引擎生成的 `__ExternalActors__`/`__ExternalObjects__` 文件。
- 每个 Agent 只编辑自己的内容目录；临时资产放 `Content/WA/Debug/<任务名>`，正式资产不得引用它们。
- 不自行 commit/push，不添加无关依赖或插件，不修改无关问题。

## 代码与资产

- 遵循 Unreal C++：Tab 缩进、Allman 大括号、`A/U/F/E/I` 类前缀和 `WA` 项目前缀。公共接口最小化，明确所有权、const 和对象生命周期。
- 不为显而易见的代码添加注释；服务器校验、网络所有权和非显然生命周期必须说明。
- 日志使用各模块 `LogWA*` 类别，不用 `printf`/`UE_LOG(LogTemp)`。
- 资产前缀：`BP_`、`WBP_`、`GA_`、`GE_`、`DA_`、`DT_`、`IA_`、`IMC_`、`ABP_`。
- 纯 C++ UMG 必须在 `NativeOnInitialized` 创建 `WidgetTree` 内容，不得在 `NativeConstruct` 才设置根控件。Python 修改 `MapKey` 返回的结构体后必须写回 `DefaultKeyMappings`，并重新读取资产确认修饰器存在。
- 二进制资产使用 Git LFS；不要提交 `Binaries`、`Intermediate`、`Saved`、`DerivedDataCache` 或 `Builds`。

## 网络底线

- 服务器权威：伤害、死亡、复活、模式、交互结果和玩家生成只能在服务器决定。
- CharacterMovement 负责移动复制；禁止每帧 Reliable RPC。Reliable 仅用于低频关键事件。
- 所有 Server RPC 检查目标有效性、距离/权限和必要频率；客户端 UI 永远不直接改权威状态。
- GameMode 只在服务器；GameState 复制模式/会话；PlayerState 拥有跨重生存续的 ASC；Character 是 Avatar。
- 主机退出结束会话是 Demo 预期行为，不实现主机迁移。

## 完成定义

1. 修改符合模块边界和文档。
2. `WorldAdventureEditor Win64 Development` 构建通过。
3. 实际运行对应场景，不能只验证 include 或编译。
4. 联机修改至少执行 PIE 两玩家验证；交付前执行打包版双机 LAN 验证。
5. 更新适用文档，清理本任务产生的无关临时文件，不删除他人文件。
