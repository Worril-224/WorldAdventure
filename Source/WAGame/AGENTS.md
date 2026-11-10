# WAGame 边界

拥有 `Source/WAGame` 和 `Content/WA/Game`。作为组合根配置默认类，负责 Host/Join/Travel、GameMode/GameState、模式伤害规则、生成和复活。GameMode 只在服务器执行；模式经 GameState 复制。不得依赖 WAUI，不得实现具体技能或探索点。共享 Config 和 Maps 仍由集成负责人修改。
