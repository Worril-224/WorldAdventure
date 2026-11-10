# WorldAdventure 框架 Demo

UE 5.8.4 C++ 工程，用于验证第三人称开放世界、2–4 人局域网合作探索和 PvP。当前使用灰盒场景和占位角色，不含正式内容。

> 当前状态：Editor 与 Win64 Development 构建通过；本机双进程合作/PvP 连接、角色生成和 HUD 已验证。主菜单点击、移动方向、攻击/复活、完整流送遍历和真实双机仍需按验收表人工确认。

## 构建

1. 安装 UE 5.8.4、Visual Studio 2022（17.8+）、MSVC 14.44、ATL、Windows SDK 和 .NET Framework 4.6.2 Developer Pack。
2. 运行 `Scripts/BuildEditor.bat`。
3. 首次生成地图和输入资产，运行 `Scripts/GenerateDemoContent.bat`。
4. 用 UE 5.8.4 打开 `WorldAdventure.uproject`，默认启动图为 `L_Demo`，游戏默认图为 `L_MainMenu`。

引擎自带 .NET 10；构建脚本已设置 `DOTNET_ROOT`，不依赖系统安装 .NET 10。

## 试玩

- 打包后可直接运行 `Builds/Windows/WorldAdventure.exe`；`Builds` 不入库，其他机器需复制完整 `Builds/Windows` 目录。
- 从主菜单选择 `Host LAN Cooperative` 或 `Host LAN PvP`。
- 另一台同局域网电脑输入主机 IPv4 地址，选择 `Join by IP`。
- `WASD` 移动，鼠标控制视角，`Space` 跳跃，鼠标左键测试攻击，`E` 激活探索点。
- PvP 每次测试攻击扣 25 血，死亡约 2 秒后由服务器复活；合作模式禁止玩家间伤害。
- 如无法加入，检查两台机器是否处于专用网络、Windows 防火墙是否允许游戏 UDP 7777，以及是否使用同一个构建。

## 规范

先读根 `AGENTS.md`，再读对应模块的 `AGENTS.md`。架构、公共接口、网络清单、多 Agent 流程和验收表位于 `Docs`。不要由多个 Agent 同时编辑共享地图、全局 Config 或公共契约。

## 当前边界

使用 Listen Server；不包含互联网/NAT、主机迁移、专用服务器、账号、存档、正式技能/特效或大地图性能承诺。主机退出会结束会话。
