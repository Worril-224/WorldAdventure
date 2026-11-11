# 内容与多 Agent 工作流

## 任务模板

每个任务必须写明：目标、非目标、唯一拥有路径、允许依赖、公共输入/输出、资产清单、网络权威、验收步骤。公共契约未落地前不得并行实现依赖它的功能。

## 分支与集成

推荐 `feature/<module>-<task>`。每次小步集成先构建 Editor，再执行对应 PIE 场景。未经用户要求不 commit/push。模块 Build.cs 由模块所有者维护；`.uproject`、Target、全局 Config、Maps 和公共契约由集成负责人维护。

## 二进制资产

- 使用 Git LFS 跟踪 uasset/umap；地图启用 World Partition/OFPA。
- 每个 Agent 只修改自己目录的资产，不同时编辑共享地图。
- 功能模块提供可放置 Actor 和参数，World/集成负责人负责放入共享地图。
- 不手工重命名外部 Actor 文件；不通过复制二进制资产来“解决”冲突。

## 原型内容生成

首次运行 `Scripts/GenerateDemoContent.bat` 后，依次运行 `Scripts/GeneratePrototypeCharacter.bat` 和 `Scripts/GeneratePrototypeWorld.bat`。角色生成器只创建颜色材质；区域生成器幂等更新 `L_Demo` 的营地、道路和三个地标，保留 GameMode、World Partition、探索点和出生点。正式内容不得引用临时 Debug 资产。

## 临时文件

临时资产放 `Content/WA/Debug/<任务名>` 且默认不入库，正式资产不得引用。构建输出在 `Builds`，缓存和日志不提交。清理自己产生的无关输出，不删除来源不明的文件。

## 提交前

构建 Editor、运行实际场景、更新契约/网络文档、检查 `git status`。联机改动至少运行两玩家 PIE；交付必须运行打包版双机 LAN 清单。
