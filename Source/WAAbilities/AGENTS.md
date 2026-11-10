# WAAbilities 边界

拥有 `Source/WAAbilities` 和 `Content/WA/Abilities`。实现 ASC、属性、GE/GA 和测试攻击。仅依赖 WACore；通过 `IWAGameRules` 查询模式/通知死亡，通过 `IWAAbilityAvatar` 请求表现，不引用 WAPlayer、WAGame 或 WAUI。伤害仅在 Authority 执行，目标 ASC 和模式必须校验。网络变化同步 `Docs/Networking.md`。
