# 模块契约

## 核心类型

- `EWAGameMode`：`Cooperative`、`PlayerVersusPlayer`。
- `IWAGameRules`：`AllowsPlayerDamage(Source, Target)` 与 `HandlePlayerDeath(Avatar)`，由服务器 GameMode 实现。
- `IWAInteractable`：交互文本和 `Interact(Interactor)`；Interact 只能经服务器调用。
- `IWAPlayerInfo`：HUD 只读生命和生命变化事件，不暴露属性写接口。
- `IWAAbilityAvatar`：测试攻击表现与死亡表现，由 Character 实现。

## Gameplay Tags

- `WA.Ability.TestAttack`：临时攻击能力。
- `WA.State.Dead`：等待服务器复活的死亡状态。

## 默认类与资产

- C++ 默认类由 `AWAGameMode` 设置，不要求蓝图子类。
- 输入资产固定路径：`/Game/WA/Characters/Input/IMC_ThirdPerson`、`IA_Move`、`IA_Look`、`IA_Jump`、`IA_Attack`、`IA_Interact`。
- 地图固定路径：`/Game/WA/Maps/L_MainMenu`、`/Game/WA/Maps/L_Demo`。
- 占位视觉直接使用 Engine BasicShapes，不引入正式角色资产。

## 跨模块规则

只 include 其他模块的 Public 头。跨模块资产使用稳定软路径或配置，不把另一模块的私有对象指针保存为长期状态。新增契约先由集成负责人落地，再开放并行任务。
