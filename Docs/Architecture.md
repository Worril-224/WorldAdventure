# 架构

## 启动与组合

`L_MainMenu` 是默认地图。`UWAGameUISubsystem` 创建主菜单；`UWASessionSubsystem` 处理 Host/Join。Host 使用 `ServerTravel('/Game/WA/Maps/L_Demo?listen?Mode=Coop|PvP')`，Join 使用局域网 IP ClientTravel。

`AWAGameMode` 配置 `AWACharacter`、`AWAPlayerController`、`AWAPlayerState`、`AWAGameState`。模式在服务器解析并写入 GameState。UI 不是 GameMode 的依赖。

## 依赖

```text
WACore
  ├─ WAAbilities ─┐
  ├─ WAWorld      ├─ WAGame ─ WAUI
  └─ WAPlayer ────┘
WAEditor 仅为编辑器工具层
```

## 玩家生命周期

PlayerState 拥有可复制 ASC 和 HealthAttributeSet，因此属性在 Character 重生时保留。服务器 Possess 和客户端 OnRep_PlayerState 都调用 `InitAbilityActorInfo(PlayerState, Character)`。能力只在服务器授予一次。

死亡由 HealthAttributeSet 检测并通过 `IWAGameRules` 通知服务器 GameMode。GameMode 延迟重生、重置属性、移除死亡 Tag、销毁旧 Pawn 并重新 Possess。

## 世界

`L_Demo` 使用 World Partition/OFPA。探索点 `AWAExplorePoint` 的激活由服务器验证和执行，`bActivated` 复制到客户端。地图只含灰盒地形、出生点和三个探索点。

## 扩展规则

正式技能继承 GAS 能力，正式角色替换网格/动画但保留玩家与 ASC 契约，正式地图沿用 World Partition，表现不得成为权威逻辑来源。
