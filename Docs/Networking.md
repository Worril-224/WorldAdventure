# 网络契约

## 权威与所有权

- Listen Server 是权威服务器；支持 2–4 人 LAN，无互联网、主机迁移和专用服务器承诺。
- GameMode 只在服务器；GameState 复制模式；PlayerState 拥有 Mixed 复制 ASC；Character 使用引擎移动复制。
- UI、相机和本地输入不复制。服务器决定生成、伤害、死亡、复活和探索点激活。

## 复制/RPC 清单

| 项目 | 所有者/调用方 | 方向与条件 | 服务器校验 |
|---|---|---|---|
| CharacterMovement | Character/服务器与拥有客户端 | UE 标准移动复制 | 引擎移动校验 |
| `AWAGameState.MatchMode` | 服务器 | ReplicatedUsing，初始与迟加入同步 | 仅 InitGameState 写入 |
| `AWAExplorePoint.bActivated` | 服务器 | ReplicatedUsing | Interact 仅 Authority，且只执行一次 |
| Health/MaxHealth | PlayerState ASC | GAS Mixed 属性复制 | 仅服务器 GE/属性重置 |
| `WA.State.Dead` | PlayerState ASC | GAS Tag 复制 | 服务器死亡/复活流程 |
| 测试攻击 | 本地 Controller 的 Character | GAS LocalPredicted 激活 | 服务器命中、距离、目标、模式、死亡状态和 0.5 秒限频 |
| `Character.ServerInteract` | 拥有客户端 | Reliable、低频按键 | 非空、流送后仍有效、300cm 距离；服务器按 Character 使用 World 时间执行 0.2 秒交互冷却 |
| `Character.MulticastAttackFeedback` | 服务器 | Unreliable，仅调试表现 | 不承载伤害 |

## 会话

- Host：`ServerTravel` 到 Demo 地图，URL 含 `listen` 和 `Mode`。
- Join：本地 Controller `ClientTravel` 到 `IP`（默认 UDP 7777）。
- Leave：普通客户端本地返回菜单；主机返回菜单会结束会话。
- Network/Travel Failure 由 SessionSubsystem 转换为 UI 消息。

## 新增网络状态要求

必须在此表记录所有者、复制条件、频率、调用方和服务器校验。禁止每帧 Reliable RPC，禁止客户端直接修改权威生命/模式/复活/其他玩家状态。
