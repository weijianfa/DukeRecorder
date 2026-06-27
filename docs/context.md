音视频业务开发工程师JD：
团队介绍: 
我们致力于构建稳定、高性能的桌面端音视频与屏幕录制基础能力，为企业级场景提供可靠的长时录制、弱网传输与大规模数据处理解决方案。 团队聚焦音视频底层能力、跨端架构与工程效率建设，通过持续的技术演进，支撑复杂业务场景下的稳定性、可扩展性与成本优化。 

岗位职责
1、负责桌面端屏幕录制系统的核心架构设计与性能优化，保障长时间录制场景下的稳定性与资源可控性； 
2、参与音视频采集、处理与传输链路建设，集成并优化第三方音视频SDK，提升弱网、丢包、 高延迟等复杂网络环境下的用户体验；
 3、设计并开发音视频相关的性能与稳定性测试工具，支持多网络条件、多负载(CPU/内存) 及多节点场景的系统验证； 
4、持续优化大体量录制文件的流式处理与资源管理策略，提高整体系统吞吐与运行效率； 
5、设计并开发Native模块，与Electron及桌面应用进行深度协作，推动跨端音视频能力的工程化落地。
职位要求
1、本科及以上学历，计算机、软件工程或相关专业优先； 
2、熟悉Electron等前端或跨端框架，具备桌面端应用开发经验； 
4、对音视频采集、编解码、多媒体系统或实时通信原理有实践经验，具备弱网处理或性能调优经验者优先； 
5、具备Windows/macOS平台开发经验，对系统性能、稳定性问题有较强分析和定位能力者优先。 
6、英语能力优先。

资深音视频业务开发专家，你是这个职位的技术面试官，请站在面试官的角度对候选人进行技术面试，要有深度、全面，采用一问一答的形式，回答要明确，同时给出为什么考核这个问题

好的，我来扮演这位**资深音视频业务开发专家面试官**，针对这个桌面端音视频与屏幕录制方向的职位进行深度技术面试。

---

## 面试开始

**面试官：** 你好，欢迎参加本次技术面试。我是音视频业务团队的资深开发专家。我们团队主要做桌面端屏幕录制、音视频采集传输以及跨端架构，今天我会围绕这几个方向深入聊聊。准备好了吗？

---

### 第一题：屏幕录制架构设计

**面试官：** 假设你需要设计一个**支持8小时以上连续录制**的桌面屏幕录制系统，要求：
1. 录制过程中CPU占用率稳定在15%以下（1080p@30fps）
2. 录制文件不能无限增长导致内存溢出
3. 支持录制过程中随时暂停/恢复，且恢复后时间戳连续

请描述你的核心架构设计，包括**采集、编码、封装、落盘**四个环节的关键决策。并说明：为什么长时间录制场景下，**"内存缓冲池 + 流式分片写入"** 比 **"整段内存缓冲后一次性写入"** 更关键？

> **考核意图：** 这是岗位核心职责第1条"长时间录制稳定性与资源可控性"的直接映射。8小时录制是典型企业级场景（会议、培训、监控），候选人是否理解"流式处理"与"资源边界控制"是区分初级与资深的关键。时间戳连续性涉及音视频同步基础，暂停/恢复是真实业务高频需求。

---

### 第二题：弱网传输与抗丢包

**面试官：** 你正在集成一个第三方RTC SDK做屏幕录制内容的实时传输。测试发现，在**模拟20%丢包、200ms抖动**的网络环境下，接收端画面频繁卡顿、花屏，但SDK自带的NACK重传机制似乎没有明显效果。

请分析：
1. **屏幕录制内容（高运动/低运动场景）** 与 **摄像头内容** 在编码特性和丢包表现上有何本质差异？这会导致NACK策略失效的原因是什么？
2. 你会从哪三个层面（编码层、传输层、应用层）分别做优化？请具体到技术方案（如编码参数、FEC策略、缓冲机制等）。

> **考核意图：** 对应岗位第2条"弱网、丢包、高延迟环境下的优化"。屏幕录制内容（PPT翻页 vs 游戏画面）编码特性差异巨大，候选人是否理解**内容自适应编码**和**屏幕内容编码（SCC）** 的特殊性，是音视频专业深度的体现。NACK对屏幕内容失效是经典陷阱题。

---

### 第三题：Native与Electron跨端协作

**面试官：** 团队技术栈是 **Electron + C++ Native模块**。现在有一个需求：Native层负责屏幕采集和H.264编码，Electron层负责UI和信令控制，两者之间需要**低延迟传输编码后的视频帧**（约2-5MB/s峰值）。

请对比以下三种方案的优劣，并给出你的选择和理由：
- **方案A：** Native通过Node-API直接传递Buffer给Electron主进程，再由主进程转发给渲染进程
- **方案B：** Native与Electron通过共享内存（Shared Memory）+ 信号量机制通信
- **方案C：** Native作为独立进程，通过localhost TCP/Unix Domain Socket传输

同时请说明：如果视频帧在传输过程中出现**内存拷贝**，会对整体CPU占用和延迟产生什么具体影响？你会如何量化评估这个影响？

> **考核意图：** 对应岗位第5条"Native模块与Electron深度协作"。跨进程通信（IPC）是桌面端音视频的核心瓶颈，2-5MB/s的码率下零拷贝设计至关重要。候选人是否理解Electron多进程架构（主进程/渲染进程/Native模块）以及内存拷贝的隐性成本，是工程化能力的关键指标。

---

### 第四题：大体量文件的流式处理

**面试官：** 用户录制了**4小时的4K屏幕内容**，生成一个**50GB的原始MP4文件**。现在需要在不加载整个文件到内存的前提下，实现以下功能：
1. 提取中间任意时间点的视频帧作为缩略图
2. 将文件按10分钟间隔切片成多个可独立播放的MP4片段
3. 在切片过程中修复可能存在的moov atom前置/后置问题

请描述你的技术方案，重点说明：
- 如何处理**MP4容器的随机访问**（moov/mdat结构）？
- 如果原始文件录制异常中断导致moov atom损坏或缺失，你的**容错/修复策略**是什么？
- 这个场景下，为什么**"流式解析 + 索引重建"** 比 **"ffmpeg -i input -c copy -f segment"** 的直接命令行切片更适合生产环境？

> **考核意图：** 对应岗位第4条"大体量录制文件的流式处理"。50GB文件处理是典型的大数据量场景，moov atom问题是录制异常中断的必现bug。候选人是否理解MP4容器格式细节、流式解析原理，以及生产环境对容错和可控性的要求，体现底层技术深度。

---

### 第五题：性能与稳定性测试工具设计

**面试官：** 岗位第3条要求"设计性能与稳定性测试工具"。假设你需要为屏幕录制系统构建一个**自动化压测平台**，要求模拟以下场景：
- **多网络条件：** 带宽限制（1Mbps-50Mbps）、丢包（0%-30%）、延迟（0ms-500ms）
- **多负载：** CPU满载（如同时运行编译任务）、内存高水位（80%+）、磁盘IO瓶颈
- **多节点：** 单机多实例并发录制、多机分布式录制

请设计这个测试平台的**架构图**（文字描述即可），并重点回答：
1. 如何**精准模拟**"CPU满载但录制进程仍需抢到足够时间片"的场景？（提示：涉及线程优先级、进程亲和性、cgroups等）
2. 如何**量化定义**"稳定性"？比如8小时录制中，什么指标组合（帧率波动、内存泄漏速率、CPU方差、文件完整性校验）可以判定"系统稳定"？请给出你的**稳定性评分模型**。

> **考核意图：** 直接对应岗位第3条。测试工具设计能力是很多开发者欠缺的，但这个岗位明确要求。候选人是否理解"压测不是功能测试"、如何构造极端边界条件、以及稳定性需要从多维度量化评估而非简单"没崩溃就行"，体现工程体系化思维。

---

### 第六题：Windows/macOS平台深度问题（选答其一）

**面试官：** 我们支持Windows和macOS双平台。请根据你的经验选择一题深入：

**Windows方向：**
> Windows DXGI桌面采集（Desktop Duplication API）在高帧率场景下，偶尔出现**采集帧与鼠标光标不同步**（鼠标已经移动，但画面中的光标位置滞后1-2帧）。请分析根因，并给出至少两种解决方案（硬件层面/软件层面）。

**macOS方向：**
> macOS从10.15开始引入ScreenCaptureKit，取代了旧的CGDisplayStream。请对比两者的**性能差异**（尤其是CPU占用和内存拷贝），并说明：在**录制高刷新率显示器（120Hz/144Hz）** 时，ScreenCaptureKit的**帧率回调机制**与旧API有何不同？如何利用新API的特性降低采集延迟？

> **考核意图：** 对应岗位第5条"Windows/macOS平台开发经验"。平台深度是桌面端开发的护城河，DXGI光标同步问题是Windows采集的经典难题，ScreenCaptureKit是macOS现代API。候选人是否跟踪平台演进、理解底层采集机制，体现技术持续投入。

---

### 第七题：综合场景与系统设计

**面试官：** 最后一题，开放设计：

假设你是这个系统的**技术负责人**，需要设计一个**"企业级屏幕录制云服务"**的端到端架构，核心需求：
- 用户桌面端发起录制，内容需要**实时上传**到云端存储
- 支持**边录边传**（录制开始后5秒内云端可播放）
- 支持**断点续传**（网络中断后恢复，不丢帧不花屏）
- 云端需要支持**实时转码**（生成多清晰度版本）和**AI内容审核**（敏感画面检测）

请画出**系统架构图**（文字描述模块和 data flow），并重点阐述：
1. **边录边传**的时延控制：本地缓存策略、上传分片大小、关键帧对齐策略
2. **断点续传**的帧级连续性保证：如何处理编码器状态、GOP边界、时间戳跳跃？
3. 如果云端转码队列积压，如何设计**背压机制**避免本地磁盘被写爆？

> **考核意图：** 综合考察候选人作为"资深专家"的**系统架构能力**和**业务理解深度**。这不是考知识点，而是考如何将前面所有技术点串联成可落地的方案。边录边传+断点续传是真实企业级需求，背压机制体现对系统鲁棒性的理解。

---

## 面试结束

**面试官：** 以上七题覆盖了从底层编码、跨端架构、容器格式、平台特性到系统设计的全链路。每道题我都给出了考核意图——我们在找的不是"能写代码的人"，而是**能定义问题、拆解问题、在约束条件下做技术决策**的资深工程师。

请你逐题回答，回答时请明确给出：**技术方案 → 关键决策理由 → 潜在风险/权衡**。准备好后可以从第一题开始。

---

> **（候选人请开始作答，面试官会根据回答深度持续追问）**

好的，我以候选人的身份来回答这次技术面试。

好的，我以候选人的身份来回答这次技术面试。

---

## 第一题：屏幕录制架构设计

**候选人：** 我先从架构整体说起，然后回答您关于内存策略的问题。

### 核心架构设计

**采集层：**
- **Windows** 用 DXGI Desktop Duplication API（Win8+）或 Graphics Capture API（Win10 1903+），优先后者支持现代应用和HDR内容
- **macOS** 用 ScreenCaptureKit（macOS 12.3+），支持高刷新率、窗口过滤、音频内录
- 采集线程独立，优先级设为 `THREAD_PRIORITY_TIME_CRITICAL`（Win）或 `QOS_CLASS_USER_INITIATED`（mac），与UI线程隔离
- 帧率控制用**可变帧率策略**：静态画面（PPT）降频到5fps，动态画面（视频播放）恢复到30fps，减少无效编码

**编码层：**
- 硬件编码优先：Intel QSV / NVIDIA NVENC / AMD VCE / Apple Media Engine
- 编码参数动态适配：
  - 码率控制：CBR（稳定网络传输）或 VBR with cap（本地录制）
  - 关键帧间隔：2秒一个IDR帧，支持随机访问
  - B帧策略：屏幕内容禁用B帧（降低延迟，简化时间戳管理），摄像头内容可开2-3个B帧

**封装层：**
- 容器用 **fMP4（Fragmented MP4）** 而非传统MP4。fMP4的moof/mdat分片结构天然支持流式写入，每个fragment独立可解析
- 分片大小：按时间分片（每2-4秒一个fragment），而非按大小分片，保证时间戳连续性
- 元数据（moov）前置生成，包含所有track信息，但sample数据随采随写

**落盘层：**
- **双缓冲异步写入**：编码线程 → 内存环形缓冲区（ring buffer）→ 独立IO线程 → 磁盘
- 文件预分配：用 `SetFileValidData`（Win）或 `ftruncate` + `F_PREALLOCATE`（mac）预先分配文件空间，避免写入过程中的磁盘碎片和延迟尖峰
- 写入策略：每完成一个fragment（2-4秒数据）立即fsync，而非每帧fsync，平衡持久性与性能

### 暂停/恢复的时间戳连续性

暂停时：
1. 停止采集线程，但**不销毁编码器上下文**，保持编码器状态
2. 记录暂停时的系统时间戳和当前DTS/PTS
3. 写入一个空的moof/mdat标记（可选，用于播放器识别断点）

恢复时：
1. 恢复采集，新帧的PTS = 恢复时系统时间 - 录制开始系统时间（单调递增）
2. 编码器继续从上次状态编码，GOP结构保持连续
3. 如果暂停时间超过关键帧间隔（如2秒），强制插入IDR帧，避免解码器参考帧缺失

### 为什么"流式分片写入"比"整段缓冲"更关键

| 维度 | 整段缓冲 | 流式分片写入 |
|------|---------|-------------|
| **内存占用** | O(n)随录制时长线性增长，8小时可能占数十GB内存 | O(1)恒定，仅缓冲2-4秒数据 |
| **崩溃恢复** | 进程崩溃则全部丢失 | 已写入磁盘的数据完整保留 |
| **磁盘IO模式** | 突发式大写入，触发系统缓存刷盘延迟 | 平稳持续写入，IO压力均匀 |
| **启动延迟** | 需等待缓冲满或录制结束才能看到文件 | 秒级可见可播放片段 |
| **系统OOM风险** | 高，长时录制必现 | 低，内存占用可控 |

**核心原因：** 长时间录制是**资源边界问题**，不是功能问题。企业级场景要求系统在**任意时刻**的资源占用都是可预测、可约束的。流式分片将"无界问题"转化为"有界问题"，这是工程化的本质。

---

## 第二题：弱网传输与抗丢包

**候选人：**

### 屏幕内容 vs 摄像头内容的编码特性差异

| 特性 | 屏幕内容（PPT/文档） | 屏幕内容（游戏/视频） | 摄像头内容 |
|------|---------------------|----------------------|-----------|
| **空间冗余** | 极高（大面积纯色、文字边缘锐利） | 中等（快速运动、纹理复杂） | 中等（自然场景、噪声） |
| **时域冗余** | 低（PPT翻页是突变） | 低（高运动） | 高（人脸微动） |
| **编码效率** | Screen Content Coding（SCC）优化显著 | 需高码率保持质量 | H.264/HEVC标准编码即可高效 |
| **帧大小波动** | 巨大（静态帧10KB，翻页帧500KB+） | 相对均匀 | 相对均匀 |
| **丢包影响** | I/P帧丢包导致大面积花屏，且错误持续时间长 | 同左，但运动掩盖部分瑕疵 | 错误可被后续帧快速掩盖 |

### NACK策略失效的根因

NACK（Negative ACKnowledgment）依赖**重传修复丢包**，但屏幕内容场景下：
1. **帧大小差异大**：一个关键帧可能500KB，在20%丢包、1Mbps带宽下，重传该帧需要4秒+，远超接收端抖动缓冲（jitter buffer）容量，导致播放端被迫跳过该帧
2. **参考帧依赖深**：屏幕内容常用长GOP（如250帧一个IDR），P帧丢包后，后续所有P帧都受影响，NACK级联重传引发**重传风暴**
3. **低时域冗余**：PPT翻页后画面长期静态，丢失的P帧没有"后续相似帧"可以掩盖错误，错误一直可见直到下一个IDR

### 三层优化方案

**编码层：**
1. **内容自适应GOP**：检测场景切换（PPT翻页、窗口切换），强制插入IDR帧，缩短错误传播范围
2. **SCC工具启用**：H.265的IBC（Intra Block Copy）、调色板模式（Palette Mode），对文字/线条编码效率提升30-50%
3. **分层编码（SVC）**：基础层（低分辨率/低帧率）优先传输，增强层可丢弃。屏幕共享时基础层保证文字可读，增强层提供清晰度
4. **动态码率**：弱网时主动降码率到内容可识别的最低阈值（如文字场景1Mbps，视频场景3Mbps），而非硬撑高码率

**传输层：**
1. **FEC（前向纠错）替代部分NACK**：对屏幕内容采用**RS-FEC或RaptorQ**，在20%丢包下，冗余20-30%即可100%恢复，避免重传延迟。FEC与NACK混合策略：小丢包用NACK，大丢包/突发丢包用FEC
2. **带宽估计与拥塞控制**：GCC（Google Congestion Control）或BBR，但针对屏幕内容调整：帧大小波动大时，带宽估计需用**峰值滤波**而非均值滤波，避免低估带宽
3. **RTP打包策略**：屏幕内容帧大，采用**MTU分片 + FU-A（H.264）或 FU-B** 而非单NAL包，减少单包丢包影响范围

**应用层：**
1. **播放端抖动缓冲动态调整**：根据网络RTT和丢包率自适应调整缓冲深度。屏幕内容对延迟不敏感（非交互场景），可容忍500ms-1s缓冲换取流畅性
2. **关键帧请求策略（PLI/FIR）优化**：丢包后不是立即请求IDR，而是先尝试NACK修复，超时后再PLI。避免频繁PLI导致码率突增
3. **内容感知降级**：检测到弱网时，主动降低分辨率（如4K→1080p）或帧率（30fps→15fps），文字场景优先保分辨率（清晰度），视频场景优先保帧率（流畅度）

---

## 第三题：Native与Electron跨端协作

**候选人：**

### 三种方案对比

| 维度 | 方案A：Node-API直接传递 | 方案B：共享内存+信号量 | 方案C：独立进程+Socket |
|------|------------------------|----------------------|----------------------|
| **延迟** | 高（主进程→渲染进程需二次拷贝） | 极低（零拷贝） | 中（内核态拷贝，但用户态零拷贝） |
| **CPU占用** | 高（Buffer在V8堆分配，大Buffer触发GC压力） | 极低 | 中（序列化/反序列化开销） |
| **内存拷贝次数** | 2-3次（Native→Node→V8→Chromium IPC） | 0次（mmap映射） | 1次（Socket内核缓冲） |
| **实现复杂度** | 低 | 高（需精细同步机制） | 中 |
| **稳定性** | 中（Native崩溃拖垮Electron主进程） | 低（同进程内，崩溃影响大） | 高（进程隔离，Native崩溃可重启） |
| **跨平台** | 好（Node-API抽象） | 中（平台差异大） | 好（TCP/UDS跨平台） |
| **Electron架构契合度** | 差（大Buffer阻塞主进程事件循环） | 差（共享内存需主进程协调） | 好（独立进程与主进程通过IPC通信） |

### 我的选择：**方案C的增强版（独立进程 + Shared Memory + Unix Domain Socket/Named Pipe）**

具体设计：
1. **Native作为独立进程**（`capture_service.exe` / `CaptureService.app`），负责采集+编码，崩溃后Electron可自动重启
2. **共享内存用于大帧传输**：Native编码完成后，将H.264 NAL单元写入命名共享内存区域（Windows: `CreateFileMapping` + `MapViewOfFile`；macOS: `shm_open` + `mmap`），仅传递偏移量+大小给Electron
3. **UDS/Named Pipe用于控制信令**：传输元数据（时间戳、帧类型、分辨率变化等），带宽极低，延迟可控
4. **Electron侧**：主进程通过Native Addon加载共享内存，渲染进程通过 `contextBridge` 获取帧数据，用于预览或WebRTC发送

### 内存拷贝的影响量化

假设1080p@30fps，H.264编码后平均帧大小500KB：

| 指标 | 有拷贝（方案A） | 零拷贝（优化方案C） |
|------|---------------|-------------------|
| **每帧拷贝数据量** | 500KB × 2次 = 1MB | ~0（仅传递指针/偏移） |
| **每秒拷贝带宽** | 30MB/s | 可忽略 |
| **CPU占用（拷贝本身）** | memcpy约占用 5-8% CPU（单核） | 0% |
| **V8 GC压力** | 大Buffer频繁分配触发GC，卡顿 | 无 |
| **端到端延迟** | 增加 5-15ms（拷贝+GC） | 增加 <1ms（仅IPC） |

**量化评估方法：**
1. **perf/VTune** 抓取CPU火焰图，观察 `memcpy`、`v8::internal::Heap::AllocateRaw` 占比
2. **自定义探针**：在Native层记录 `PostFrame` 到Electron层 `OnFrame` 的时间差，对比有无拷贝
3. **内存分析**：Electron DevTools Memory + Node `--heapsnapshot-near-heap-limit` 观察GC频率

---

## 第四题：大体量文件的流式处理

**候选人：**

### 技术方案架构

```
50GB MP4文件
    ↓
[流式解析器] → 按需读取文件头（moov/moov atom）→ 构建sample索引表
    ↓
[索引重建模块] → 解析所有trak的stsz/stco/stts/stsc → 建立时间→字节偏移映射
    ↓
[缩略图提取] → 定位目标时间最近的IDR帧 → 解码单帧 → 缩放 → 输出
    ↓
[切片引擎] → 按10分钟边界切割GOP → 生成独立moov + mdat片段 → 验证播放
    ↓
[moov修复模块] → 检测atom位置 → 前置/后置修复 → 完整性校验
```

### MP4随机访问处理

MP4文件结构核心是 `moov`（元数据）和 `mdat`（媒体数据）。随机访问依赖 `moov` 中的 `stco`（chunk偏移）和 `stsz`（sample大小）表。

**关键操作：**
1. **流式解析**：不加载整个文件，用 `mmap` 映射文件头区域（通常前几十MB），解析moov结构
2. **时间→字节映射**：遍历 `stts`（时间→sample映射）和 `stsc`（sample→chunk映射），构建 `time → file_offset` 索引表。50GB文件索引表内存占用约 **50-100MB**（取决于track数量），完全可接受
3. **缩略图提取**：给定目标时间T，二分查找索引表找到最近的IDR帧（通过 `stss` 表识别），`seek` 到对应偏移，读取一个GOP数据，送解码器解码第一帧

### moov atom损坏修复策略

**场景1：moov后置且完整**
- 正常MP4要求moov前置（streaming-friendly），但某些录制工具后置moov
- 修复：解析文件尾部找到moov，提取trak信息，重新生成前置moov，拼接mdat数据

**场景2：moov部分损坏（如stco表缺失）**
-  Fallback到 **mdat流式扫描**：逐字节扫描mdat区域，识别NAL单元起始码（0x000001或0x00000001），重建帧边界
-  解析SPS/PPS中的timing_info（VUI参数），重建时间戳
-  生成新的moov，标记为"修复后文件"

**场景3：录制异常中断，moov完全缺失**
- 这是最常见的企业级问题（用户强制关机、进程崩溃、磁盘满）
- 修复策略：
  1. **NAL扫描**：从文件头开始扫描H.264/H.265 NAL单元，识别IDR帧（NAL type 5）
  2. **时间戳重建**：如果NAL单元中有SEI帧时间戳，优先使用；否则按帧率假设均匀分布
  3. **生成最小moov**：包含trak、mdia、minf、stbl等必需box，stco/stsz通过扫描结果填充
  4. **完整性标记**：修复后的文件头部写入 `udta` box 标记修复时间和策略，便于后续审计

### 为什么"流式解析+索引重建"比ffmpeg直接切片更适合生产

| 维度 | `ffmpeg -c copy -f segment` | 流式解析+索引重建 |
|------|---------------------------|------------------|
| **启动速度** | 需扫描整个文件建立索引，50GB文件需数分钟 | 按需解析，秒级启动 |
| **内存占用** | 默认缓冲整个GOP，大GOP时内存峰值高 | 恒定内存，逐帧处理 |
| **容错性** | 遇到损坏moov直接失败，无修复能力 | 内置修复逻辑，降级处理 |
| **可控性** | 黑盒，错误信息不透明 | 每步可监控、可干预、可回滚 |
| **切片精度** | 按GOP切割，边界可能偏离目标时间 ±2秒 | 精确到IDR帧，支持强制IDR对齐 |
| **扩展性** | 难以集成业务逻辑（如水印、加密） | 模块化设计，易扩展 |

**生产环境核心诉求是"可控"和"可观测"**。ffmpeg是工具，不是服务。50GB文件处理中任何异常（内存不足、磁盘IO错误、格式不兼容）都需要业务层捕获、降级、重试，这要求我们对每个字节有完全掌控。

---

## 第五题：性能与稳定性测试工具设计

**候选人：**

### 测试平台架构

```
[测试调度中心] (Electron/Web UI)
    ↓
[节点控制器] × N (Windows/macOS物理机/VM)
    ↓
┌─────────────┬─────────────┬─────────────┐
│  网络模拟层   │  负载注入层   │  被测录制进程  │
│ (tc/netem)   │ (stress-ng) │ (ScreenRecorder)│
└─────────────┴─────────────┴─────────────┘
    ↓
[数据采集器] → Prometheus + Grafana
    ↓
[稳定性评分引擎] → 报告生成
```

### 精准模拟"CPU满载但录制进程抢到时间片"

**核心矛盾：** 普通CPU压测（如 `stress-ng --cpu 32`）会让录制进程与其他进程公平竞争，但真实场景是录制进程需要**相对高优先级**保证实时性。我们要测的是"在恶劣环境下，系统是否仍能保证录制质量"。

**方案：**
1. **进程优先级隔离**：
   - 压测进程： `IDLE_PRIORITY_CLASS`（Win）或 `SCHED_IDLE`（mac/Linux），让它们只在CPU空闲时跑
   - 录制进程：保持正常优先级，但通过 `SetThreadPriority` 将采集/编码线程设为 `THREAD_PRIORITY_TIME_CRITICAL`
   - 这样CPU利用率显示100%，但录制线程仍能抢到时间片

2. **CPU亲和性控制**：
   - 将压测进程绑定到CPU 0-5，录制进程绑定到CPU 6-7（假设8核）
   - 模拟"部分CPU核心满载，录制进程在专属核心运行"的场景
   - 更极端：录制进程与压测进程共享核心，但录制进程优先级更高，测试调度器是否公平

3. **cgroup v2（Linux VM）/ Job Objects（Windows）**：
   - 限制压测进程的CPU quota（如80%），强制留下20%给录制进程
   - 测试录制进程在"配额CPU"下的表现

### 稳定性评分模型

**多维度指标体系：**

| 一级指标 | 二级指标 | 权重 | 合格线 | 测量方法 |
|---------|---------|------|--------|---------|
| **帧质量** | 实际帧率/目标帧率 | 20% | ≥95% | 解析输出文件帧间隔 |
| | 帧间隔方差（jitter） | 10% | ≤50ms² | 计算相邻帧PTS差的标准差 |
| **资源健康** | 内存泄漏速率 | 20% | ≤1MB/小时 | 每小时采样RSS，线性拟合斜率 |
| | CPU占用方差 | 10% | ≤(均值×20%)² | 1分钟窗口内的CPU%标准差 |
| | 句柄/GDI泄漏 | 5% | 零增长 | Process Explorer监控 |
| **文件完整性** | 文件可播放性 | 15% | 100% | ffprobe验证moov完整性 |
| | 音画同步偏移 | 10% | ≤40ms | 解析AV同步差 |
| | 时间戳连续性 | 10% | 无跳跃 | 检查PTS单调递增 |

**稳定性评分公式：**

```
Score = Σ(指标归一化值 × 权重)

归一化规则：
- 帧率达标率：实际/目标，上限1.0
- 内存泄漏：1 - min(泄漏速率/10MBh, 1.0)  # 10MB/h为崩溃阈值
- 帧间隔方差：1 - min(方差/100ms², 1.0)
- 文件可播放性：二元，0或1

评级：
- S级（95-100）：企业级生产可用
- A级（85-94）：小瑕疵，需优化
- B级（70-84）：明显问题，不可上线
- C级（<70）：严重缺陷，需重构
```

**8小时录制通过标准：** 所有一级指标持续达标，且评分 ≥ S级阈值。任一指标触发"致命线"（如内存泄漏>5MB/h、文件不可播放）立即终止并标记失败。

---

## 第六题：Windows平台深度问题（DXGI光标同步）

**候选人：** 我选择Windows方向。

### 根因分析

DXGI Desktop Duplication API 返回的帧数据是**桌面纹理**，而鼠标光标是**Overlay层**，由 `IDXGIOutputDuplication::GetFramePointerShape` 单独提供光标位图和位置。

**不同步的根因：**

1. **采样时序差异**：
   - 桌面纹理更新：由GPU完成，频率与显示器刷新率一致（60Hz/144Hz）
   - 光标更新：由Windows输入子系统处理，频率与鼠标轮询率一致（通常125Hz-1000Hz）
   - 当鼠标轮询率（如1000Hz）高于显示刷新率（60Hz）时，光标位置在两次桌面帧之间更新了16次，但桌面纹理只捕获了其中某一个状态

2. **合成时序差异**：
   - `AcquireNextFrame` 获取的是"上一帧桌面内容 + 光标位置快照"
   - 但光标位图（pointer shape）和位置（pointer position）是分开获取的，存在微小时间差
   - 高帧率录制时（如120fps采集60Hz桌面），两次采集可能拿到**同一桌面纹理但不同光标位置**，导致光标"跳跃"或"滞后"

3. **GPU与CPU时钟偏差**：
   - 纹理时间戳来自GPU时钟，光标位置时间戳来自CPU `QueryPerformanceCounter`，两者存在漂移

### 两种解决方案

**方案一：软件层同步（推荐，通用性强）**

核心思路：**不依赖DXGI提供的光标，自行合成光标**

1. **Hook鼠标输入**：用 `SetWindowsHookEx(WH_MOUSE_LL)` 或 Raw Input 捕获鼠标移动事件，获取高精度光标位置
2. **时间戳对齐**：记录鼠标事件的时间戳（`QPC`），与桌面纹理的 `DXGI_OUTDUPL_FRAME_INFO::LastPresentTime` 对齐
3. **自定义绘制**：
   - 获取光标位图（`GetCursorInfo` + `DrawIconEx` 到内存DC）
   - 在桌面纹理渲染到最终帧时，根据时间戳差值插值光标位置
   - 如果鼠标事件时间戳晚于纹理时间戳，说明光标已移动但桌面未更新，此时**不绘制光标**或**预测位置**（基于速度向量外推）

4. **多帧缓冲平滑**：
   - 维护光标位置环形缓冲区（最近100ms）
   - 采集帧时，找到与纹理时间戳最接近的鼠标位置，而非最新位置
   - 避免光标"超前"于桌面内容

**方案二：硬件层规避（特定场景）**

核心思路：**让光标成为桌面纹理的一部分，消除分层**

1. **禁用硬件光标**：通过注册表或API设置，强制Windows使用软件光标渲染（`SystemParametersInfo(SPI_SETMOUSETRAILS, ...)` 间接影响，或修改显卡驱动设置）
   - 缺点：影响用户体验，非录制期间也生效，不可接受

2. **替代采集方案**：
   - 对于全屏游戏/应用，改用 **GPU-PassThrough** 或 **BitBlt** 采集（`PrintWindow` + `PW_RENDERFULLCONTENT`），此时光标已合成在画面中
   - 缺点：性能差，不支持现代UWP/DirectComposition应用

3. **Windows Graphics Capture API（Win10 1903+）**：
   - 新API `GraphicsCaptureItem` 自动处理光标合成，微软在系统层解决了同步问题
   - **这是长期推荐方案**，但需确认目标用户系统版本覆盖率

**我的选择：** 生产环境采用 **方案一（软件层同步）作为 fallback**，同时 **优先使用 Graphics Capture API**。对于必须支持Win7/8的老版本，软件层同步经过充分测试后上线。

---

## 第七题：综合场景与系统设计

**候选人：**

### 系统架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                         用户桌面端 (Electron + Native)              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │  UI/信令控制  │  │  屏幕采集+编码 │  │  流式上传客户端 (Uploader) │  │
│  │  (Electron)  │  │  (Native)     │  │  (C++ / Node-addon)      │  │
│  └──────┬──────┘  └──────┬──────┘  └───────────┬─────────────┘  │
│         │                │                     │                │
│         └────────────────┴─────────────────────┘                │
│                          │                                       │
│                    [本地磁盘缓存] (环形Buffer, 保留最近5分钟)        │
│                          │                                       │
└──────────────────────────┼─────────────────────────────────────┘
                           │ HTTPS/WSS (QUIC优先)
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                         云端接入层                                │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │  边缘节点(LB) │  │  信令网关     │  │  上传接收服务 (Upload Sink)│  │
│  │  (Anycast)   │  │  (WebSocket)  │  │  (分片重组、校验、落盘)     │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
    ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
    │  对象存储      │ │  实时转码队列  │ │  AI审核管道   │
    │  (S3/MinIO)  │ │  (Kafka+FFmpeg)│ │  (帧提取+模型)│
    │  原始文件存储  │ │  多清晰度输出  │ │  敏感内容检测  │
    └─────────────┘ └─────────────┘ └─────────────┘
```

### 边录边传的时延控制

**本地缓存策略：**
- **双缓冲设计**：Buffer A（写入中，2秒数据） + Buffer B（上传中，2秒数据）
- 编码器完成一个GOP（如2秒）后，触发Buffer切换，Buffer B进入上传队列
- 磁盘缓存作为三级缓冲：内存双缓冲满时溢写到磁盘，防止上传速度暂时低于录制速度

**上传分片大小：**
- 视频分片：按 **GOP边界对齐**，每个分片 = 1-2个GOP（2-4秒），大小通常 500KB-2MB
- 过小（<100KB）：HTTP请求头开销占比高，TCP慢启动影响大
- 过大（>5MB）：丢包重传代价高，且延迟增加
- 自适应：网络RTT < 100ms且带宽充足时，分片可增大到5MB；RTT > 300ms时，缩小到500KB

**关键帧对齐：**
- 每个上传分片必须以IDR帧开头，确保云端收到分片后可独立解码播放
- 云端播放器维护 **分片索引表**：`[分片序号, 起始时间, 结束时间, 文件偏移, 是否IDR开头]`
- 用户请求"从第3秒播放"时，云端找到包含第3秒的分片（可能从第2秒开始），从IDR帧解码，前端丢弃前1秒画面，实现秒开

### 断点续传的帧级连续性

**核心挑战：** 网络中断后，编码器状态（参考帧、DTS/PTS计数器）必须保持连续，否则恢复后出现花屏或时间戳跳变。

**方案：**

1. **编码器状态持久化**：
   - 每次上传分片成功ACK后，记录当前编码器状态：最后一个输出的NAL单元类型、DTS/PTS值、参考帧队列的POC（Picture Order Count）
   - 状态写入本地SQLite，与分片一一对应

2. **GOP边界对齐恢复**：
   - 网络中断时，编码器继续运行（或暂停，取决于业务策略），但丢弃输出直到网络恢复
   - 恢复后，读取SQLite中的状态，**从下一个IDR帧开始上传**
   - 如果中断期间编码器未暂停，则中断期间的P帧不传输，只传IDR帧，确保云端解码器参考帧正确

3. **时间戳连续性**：
   - PTS/DTS使用**录制开始后的绝对时间**（单调递增），而非相对时间
   - 中断期间时间戳继续递增（即使不传输），恢复后时间戳自然连续
   - 云端播放器通过 `baseMediaDecodeTime`（fMP4的tfdt box）识别时间戳跳跃，自动调整播放时间轴

4. **文件级修复**：
   - 云端对象存储中的文件是**追加写**模式，每个分片作为一个segment
   - 中断后，云端校验最后一个segment的完整性（md5），请求客户端从该segment重传或续传

### 背压机制（防止本地磁盘写爆）

**问题：** 云端转码队列积压 → 原始文件上传速度被迫降低（或停止）→ 本地录制继续产生数据 → 磁盘满 → 系统崩溃

**三级背压：**

1. **上传客户端背压（第一级）**：
   - 监控上传队列深度（待上传分片数 × 平均分片大小）
   - 队列深度 > 阈值（如50MB）时，向Native层发送 **PAUSE_ENCODING** 信号
   - Native层收到信号后：暂停采集线程，编码器输出缓冲到内存，停止生成新帧
   - 队列深度 < 阈值/2 时，发送 **RESUME_ENCODING**

2. **本地磁盘配额（第二级）**：
   - 磁盘缓存目录设置硬上限（如5GB）
   - 达到80%配额时，触发**自动降质**：降低录制分辨率（4K→1080p）或帧率（30fps→15fps），减少数据产生速率
   - 达到95%配额时，**强制停止录制**，UI提示用户"云端处理延迟，本地存储不足"

3. **云端队列感知（第三级）**：
   - 上传客户端与云端信令网关保持心跳，获取云端转码队列深度
   - 云端队列深度 > 1000任务时，上传客户端主动降速（如限制上传带宽为标称值的50%），给云端喘息时间
   - 与云端协商 **动态码率**：云端告知"当前可用带宽"，客户端调整编码码率匹配

**关键设计原则：** 背压信号必须**逐级传递**，从云端 → 上传客户端 → 编码器 → 采集器，每一层都有明确的降级策略，避免单点阻塞导致级联故障。

---

## 总结

**候选人：** 以上是我对七个问题的回答。整体思路是围绕**"企业级可靠性"**展开：长时间录制要求资源有界、弱网要求自适应降级、跨端要求零拷贝、大体量文件要求流式可控、测试要求量化评估、平台要求深度理解、系统设计要求端到端背压。这些不是独立的技术点，而是同一个工程体系的不同切面。

---

**（等待面试官追问）**

现在请你以资深音视频开发专家根据以上的回答手搓一个桌面录制工程吧，先规划后实现

 好的，我来以**资深音视频开发专家**的身份，基于刚才面试中阐述的技术方案，手搓一个**桌面屏幕录制工程**。

---

## 一、工程规划

### 1.1 项目定位

**DukeRecorder** —— 企业级桌面屏幕录制引擎，核心特性：
- **跨平台**：Windows 10+ / macOS 12+
- **长时间录制**：8小时+，内存占用恒定，流式分片落盘
- **边录边传**：fMP4流式分片，支持云端实时播放
- **弱网自适应**：动态码率、GOP调整、断点续传
- **零拷贝跨端**：Electron + Native 共享内存通信

### 1.2 技术栈选型

| 模块 | 技术选型 | 理由 |
|------|---------|------|
| 采集 | Windows: DXGI + Graphics Capture API<br>macOS: ScreenCaptureKit | 现代API，性能最优 |
| 编码 | VideoToolbox (mac) / MediaFoundation (Win) / x264 fallback | 硬件编码优先，保证低CPU |
| 封装 | 自研 fMP4 Muxer | 流式分片、moov前置、时间戳精确控制 |
| IPC | 共享内存 + UDS/Named Pipe | 零拷贝大帧传输 |
| 控制 | gRPC/Protobuf over UDS | 强类型信令，易扩展 |
| 日志 | spdlog | 高性能异步日志 |
| 构建 | CMake + vcpkg | 跨平台统一 |

### 1.3 目录结构

```
DukeRecorder/
├── CMakeLists.txt
├── cmake/
│   └── dependencies.cmake
├── src/
│   ├── core/                    # 平台抽象层
│   │   ├── platform.h           # 平台检测、基础类型
│   │   ├── ring_buffer.h/.cpp   # 无锁环形缓冲区
│   │   ├── thread_pool.h/.cpp   # 线程池
│   │   └── timer.h/.cpp         # 高精度计时
│   ├── capture/                 # 采集模块
│   │   ├── capture_engine.h     # 采集引擎接口
│   │   ├── dxgi_capture.h/.cpp  # Windows DXGI实现
│   │   ├── graphics_capture.h/.cpp # Windows Graphics Capture
│   │   └── sck_capture.h/.mm    # macOS ScreenCaptureKit
│   ├── encode/                  # 编码模块
│   │   ├── encode_engine.h      # 编码引擎接口
│   │   ├── hw_encoder.h/.cpp    # 硬件编码抽象
│   │   ├── mf_encoder.h/.cpp    # MediaFoundation
│   │   ├── vt_encoder.h/.mm     # VideoToolbox
│   │   └── x264_encoder.h/.cpp  # 软件fallback
│   ├── mux/                     # 封装模块
│   │   ├── fmp4_muxer.h/.cpp    # fMP4流式封装
│   │   ├── mp4_parser.h/.cpp    # MP4解析/修复
│   │   └── atom_writer.h/.cpp   # Box写入器
│   ├── ipc/                     # 跨进程通信
│   │   ├── ipc_channel.h        # IPC接口
│   │   ├── shared_memory.h/.cpp # 共享内存
│   │   ├── named_pipe.h/.cpp    # Windows Named Pipe
│   │   └── unix_socket.h/.cpp   # Unix Domain Socket
│   ├── upload/                  # 上传模块
│   │   ├── upload_engine.h/.cpp # 上传引擎
│   │   ├── chunk_manager.h/.cpp # 分片管理
│   │   └── http_client.h/.cpp   # HTTP/2客户端
│   └── main.cpp                 # 服务入口
├── include/                     # 对外C API
│   └── duke_recorder.h
├── proto/                       # Protobuf定义
│   └── control.proto
├── tests/                       # 单元测试
│   └── ...
└── electron/                    # Electron绑定
    ├── binding.gyp
    └── src/
        └── native_addon.cpp
```

---

## 二、核心模块实现

### 2.1 平台抽象与基础工具

**`src/core/platform.h`** —— 平台检测与基础宏

```cpp
#pragma once

#ifdef _WIN32
    #define DUKE_PLATFORM_WINDOWS 1
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <d3d11.h>
    #include <dxgi1_2.h>
#else
    #define DUKE_PLATFORM_MACOS 1
    #include <TargetConditionals.h>
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace duke {

// 高精度时间戳（微秒）
using TimestampUs = int64_t;
inline TimestampUs NowUs() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

// 视频帧描述
struct FrameDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t stride = 0;      // 行字节数
    uint32_t format = 0;      // FourCC: NV12, BGRA, etc.
    TimestampUs timestamp_us = 0;  // 采集时间戳
    TimestampUs duration_us = 0;   // 帧持续时间
    bool is_keyframe = false;
};

// 编码帧
struct EncodedFrame {
    std::vector<uint8_t> data;
    FrameDesc desc;
    int64_t pts = 0;          //  presentation timestamp
    int64_t dts = 0;          //  decode timestamp
};

// 录制配置
struct RecorderConfig {
    uint32_t width = 1920;
    uint32_t height = 1080;
    uint32_t fps = 30;
    uint32_t bitrate_kbps = 4000;
    uint32_t gop_seconds = 2;       // GOP长度（秒）
    uint32_t fragment_seconds = 4;  // fMP4分片长度（秒）
    std::string output_path;        // 本地输出路径
    bool enable_streaming = true;   // 是否启用流式上传
    std::string upload_endpoint;    // 上传地址
};

// 通用回调
using ErrorCallback = std::function<void(int code, const std::string& msg)>;
using FrameCallback = std::function<void(const EncodedFrame& frame)>;

} // namespace duke
```

**`src/core/ring_buffer.h`** —— 无锁环形缓冲区（采集→编码→封装流水线核心）

```cpp
#pragma once
#include "platform.h"

namespace duke {

// 无锁单生产者单消费者环形缓冲区
// 用于：采集线程 → 编码线程 的高吞吐数据传输
template<size_t Capacity>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0) {}

    // 生产者：写入数据
    bool Write(const uint8_t* data, size_t len) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t head = head_.load(std::memory_order_acquire);
        
        if (AvailableWrite(head, tail) < len) return false;
        
        for (size_t i = 0; i < len; ++i) {
            buffer_[(tail + i) % Capacity] = data[i];
        }
        tail_.store((tail + len) % Capacity, std::memory_order_release);
        return true;
    }

    // 消费者：读取数据
    bool Read(uint8_t* out, size_t len) {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_.load(std::memory_order_acquire);
        
        if (AvailableRead(head, tail) < len) return false;
        
        for (size_t i = 0; i < len; ++i) {
            out[i] = buffer_[(head + i) % Capacity];
        }
        head_.store((head + len) % Capacity, std::memory_order_release);
        return true;
    }

    size_t AvailableRead() const {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail + Capacity - head) % Capacity;
    }

private:
    size_t AvailableWrite(size_t head, size_t tail) const {
        return (head + Capacity - tail - 1) % Capacity;
    }
    size_t AvailableRead(size_t head, size_t tail) const {
        return (tail + Capacity - head) % Capacity;
    }

    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
    uint8_t buffer_[Capacity];
};

// 视频帧专用缓冲区（支持变长帧）
class FrameBuffer {
public:
    struct Slot {
        std::atomic<bool> ready{false};
        std::atomic<bool> consumed{false};
        std::vector<uint8_t> data;
        FrameDesc desc;
    };

    explicit FrameBuffer(size_t slot_count = 8, size_t max_frame_size = 10 * 1024 * 1024)
        : slots_(slot_count), max_frame_size_(max_frame_size) {
        for (auto& slot : slots_) {
            slot.data.reserve(max_frame_size);
        }
    }

    // 获取可写入的slot（阻塞直到有空间）
    Slot* AcquireWriteSlot() {
        while (true) {
            for (auto& slot : slots_) {
                if (!slot.ready.load(std::memory_order_acquire)) {
                    return &slot;
                }
            }
            // 所有slot都在使用中，短暂自旋后重试
            std::this_thread::yield();
        }
    }

    // 标记写入完成
    void CommitWrite(Slot* slot) {
        slot->ready.store(true, std::memory_order_release);
    }

    // 获取可读取的slot（非阻塞）
    Slot* AcquireReadSlot() {
        for (auto& slot : slots_) {
            if (slot.ready.load(std::memory_order_acquire) && 
                !slot.consumed.load(std::memory_order_relaxed)) {
                return &slot;
            }
        }
        return nullptr;
    }

    // 标记读取完成，释放slot
    void ReleaseReadSlot(Slot* slot) {
        slot->consumed.store(false, std::memory_order_relaxed);
        slot->ready.store(false, std::memory_order_release);
    }

private:
    std::vector<Slot> slots_;
    size_t max_frame_size_;
};

} // namespace duke
```

---

### 2.2 采集模块

**`src/capture/capture_engine.h`** —— 采集引擎抽象接口

```cpp
#pragma once
#include "../core/platform.h"

namespace duke {

// 采集回调：每帧原始数据
using RawFrameCallback = std::function<void(const uint8_t* data, const FrameDesc& desc)>;

class CaptureEngine {
public:
    virtual ~CaptureEngine() = default;
    
    virtual bool Initialize(const RecorderConfig& config) = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual void SetCallback(RawFrameCallback callback) = 0;
    
    // 获取推荐编码参数（基于实际采集内容）
    virtual void GetEncoderParams(uint32_t& width, uint32_t& height, uint32_t& fps) = 0;
};

// 工厂函数
std::unique_ptr<CaptureEngine> CreateCaptureEngine();

} // namespace duke
```

**`src/capture/dxgi_capture.h/.cpp`** —— Windows DXGI Desktop Duplication

```cpp
// dxgi_capture.h
#pragma once
#include "capture_engine.h"

#ifdef DUKE_PLATFORM_WINDOWS

#include <wrl/client.h>

namespace duke {

class DxgiCaptureEngine : public CaptureEngine {
public:
    DxgiCaptureEngine();
    ~DxgiCaptureEngine() override;

    bool Initialize(const RecorderConfig& config) override;
    bool Start() override;
    bool Stop() override;
    void SetCallback(RawFrameCallback callback) override;
    void GetEncoderParams(uint32_t& width, uint32_t& height, uint32_t& fps) override;

private:
    void CaptureThread();
    bool ProcessFrame();
    bool InitD3D11();
    
    // 光标合成（解决DXGI光标不同步问题）
    void ComposeCursor(uint8_t* frame_data, uint32_t pitch, 
                       const DXGI_OUTDUPL_FRAME_INFO& frame_info);

    RecorderConfig config_;
    RawFrameCallback callback_;
    std::atomic<bool> running_{false};
    std::thread capture_thread_;

    // D3D11 / DXGI
    Microsoft::WRL::ComPtr<ID3D11Device> d3d_device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d_context_;
    Microsoft::WRL::ComPtr<IDXGIOutputDuplication> duplication_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> staging_texture_;
    
    D3D11_TEXTURE2D_DESC texture_desc_{};
    uint32_t output_width_ = 0;
    uint32_t output_height_ = 0;
    
    // 光标状态
    std::mutex cursor_mutex_;
    std::vector<uint8_t> cursor_bitmap_;
    DXGI_OUTDUPL_POINTER_SHAPE_INFO cursor_info_{};
    POINT cursor_position_{0, 0};
    bool cursor_visible_ = false;
    
    // 性能统计
    std::atomic<uint64_t> frame_count_{0};
    TimestampUs start_time_us_ = 0;
};

} // namespace duke

#endif // DUKE_PLATFORM_WINDOWS
```

```cpp
// dxgi_capture.cpp
#include "dxgi_capture.h"

#ifdef DUKE_PLATFORM_WINDOWS

#include <d3d11.h>
#include <dxgi1_2.h>
#include <spdlog/spdlog.h>

namespace duke {

DxgiCaptureEngine::DxgiCaptureEngine() = default;
DxgiCaptureEngine::~DxgiCaptureEngine() {
    Stop();
}

bool DxgiCaptureEngine::Initialize(const RecorderConfig& config) {
    config_ = config;
    
    if (!InitD3D11()) {
        spdlog::error("Failed to initialize D3D11");
        return false;
    }
    
    // 创建staging texture用于CPU读取
    D3D11_TEXTURE2D_DESC desc = texture_desc_;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags = 0;
    
    HRESULT hr = d3d_device_->CreateTexture2D(&desc, nullptr, &staging_texture_);
    if (FAILED(hr)) {
        spdlog::error("Failed to create staging texture: 0x{:X}", hr);
        return false;
    }
    
    spdlog::info("DXGI capture initialized: {}x{}@{}", 
                 output_width_, output_height_, config_.fps);
    return true;
}

bool DxgiCaptureEngine::InitD3D11() {
    HRESULT hr = S_OK;
    
    // 创建D3D11设备
    D3D_FEATURE_LEVEL feature_level;
    hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, D3D11_SDK_VERSION,
        &d3d_device_, &feature_level, &d3d_context_
    );
    if (FAILED(hr)) {
        spdlog::warn("Hardware D3D11 failed, trying WARP...");
        hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            nullptr, 0, D3D11_SDK_VERSION,
            &d3d_device_, &feature_level, &d3d_context_
        );
        if (FAILED(hr)) return false;
    }

    // 获取DXGI输出
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
    hr = d3d_device_->QueryInterface(IID_PPV_ARGS(&dxgi_device));
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    hr = dxgi_device_->GetAdapter(&adapter);
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IDXGIOutput> output;
    hr = adapter_->EnumOutputs(0, &output);  // 主显示器
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IDXGIOutput1> output1;
    hr = output_->QueryInterface(IID_PPV_ARGS(&output1));
    if (FAILED(hr)) return false;

    // 创建桌面复制
    hr = output1_->DuplicateOutput(d3d_device_.Get(), &duplication_);
    if (FAILED(hr)) {
        if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) {
            spdlog::error("Desktop Duplication already in use by another application");
        }
        return false;
    }

    // 获取输出描述
    DXGI_OUTPUT_DESC output_desc;
    output->GetDesc(&output_desc);
    output_width_ = output_desc.DesktopCoordinates.right - output_desc.DesktopCoordinates.left;
    output_height_ = output_desc.DesktopCoordinates.bottom - output_desc.DesktopCoordinates.top;

    // 创建纹理描述
    texture_desc_.Width = output_width_;
    texture_desc_.Height = output_height_;
    texture_desc_.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texture_desc_.ArraySize = 1;
    texture_desc_.MipLevels = 1;
    texture_desc_.SampleDesc.Count = 1;
    texture_desc_.Usage = D3D11_USAGE_DEFAULT;
    texture_desc_.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    return true;
}

bool DxgiCaptureEngine::Start() {
    if (running_.exchange(true)) return false;
    
    start_time_us_ = NowUs();
    capture_thread_ = std::thread(&DxgiCaptureEngine::CaptureThread, this);
    
    // 设置线程优先级为时间关键
#ifdef DUKE_PLATFORM_WINDOWS
    SetThreadPriority(capture_thread_.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
#endif
    
    spdlog::info("DXGI capture started");
    return true;
}

bool DxgiCaptureEngine::Stop() {
    if (!running_.exchange(false)) return false;
    
    if (capture_thread_.joinable()) {
        capture_thread_.join();
    }
    
    // 清理资源
    duplication_.Reset();
    staging_texture_.Reset();
    d3d_context_.Reset();
    d3d_device_.Reset();
    
    spdlog::info("DXGI capture stopped, total frames: {}", frame_count_.load());
    return true;
}

void DxgiCaptureEngine::SetCallback(RawFrameCallback callback) {
    callback_ = std::move(callback);
}

void DxgiCaptureEngine::GetEncoderParams(uint32_t& width, uint32_t& height, uint32_t& fps) {
    width = output_width_;
    height = output_height_;
    fps = config_.fps;
}

void DxgiCaptureEngine::CaptureThread() {
    // 帧率控制
    const auto frame_interval = std::chrono::microseconds(1000000 / config_.fps);
    auto next_frame_time = std::chrono::steady_clock::now();

    while (running_) {
        auto now = std::chrono::steady_clock::now();
        if (now < next_frame_time) {
            std::this_thread::sleep_until(next_frame_time);
        }
        next_frame_time += frame_interval;

        if (!ProcessFrame()) {
            // 帧获取失败，短暂等待
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

bool DxgiCaptureEngine::ProcessFrame() {
    if (!duplication_) return false;

    Microsoft::WRL::ComPtr<IDXGIResource> resource;
    DXGI_OUTDUPL_FRAME_INFO frame_info;
    
    // 获取帧，超时100ms
    HRESULT hr = duplication_->AcquireNextFrame(100, &frame_info, &resource);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        return false;  // 无新帧（屏幕无变化）
    }
    if (FAILED(hr)) {
        spdlog::error("AcquireNextFrame failed: 0x{:X}", hr);
        return false;
    }

    // 获取纹理
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    hr = resource->QueryInterface(IID_PPV_ARGS(&texture));
    if (FAILED(hr)) {
        duplication_->ReleaseFrame();
        return false;
    }

    // 复制到staging texture
    d3d_context_->CopyResource(staging_texture_.Get(), texture.Get());

    // 映射读取
    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = d3d_context_->Map(staging_texture_.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr)) {
        duplication_->ReleaseFrame();
        return false;
    }

    // 构建帧描述
    FrameDesc desc;
    desc.width = output_width_;
    desc.height = output_height_;
    desc.stride = mapped.RowPitch;
    desc.format = 'BGRA';  // FourCC
    desc.timestamp_us = NowUs();
    desc.duration_us = 1000000 / config_.fps;
    
    // 光标合成（解决不同步问题）
    // 策略：如果检测到光标位置变化，在CPU侧合成到帧数据中
    if (frame_info.LastMouseUpdateTime.QuadPart != 0) {
        ComposeCursor(static_cast<uint8_t*>(mapped.pData), mapped.RowPitch, frame_info);
    }

    // 回调
    if (callback_) {
        callback_(static_cast<uint8_t*>(mapped.pData), desc);
    }

    d3d_context_->Unmap(staging_texture_.Get(), 0);
    duplication_->ReleaseFrame();
    
    frame_count_++;
    return true;
}

void DxgiCaptureEngine::ComposeCursor(uint8_t* frame_data, uint32_t pitch,
                                      const DXGI_OUTDUPL_FRAME_INFO& frame_info) {
    // 获取光标形状（如果变化）
    if (frame_info.PointerShapeBufferSize > 0) {
        cursor_bitmap_.resize(frame_info.PointerShapeBufferSize);
        UINT required_size = 0;
        duplication_->GetFramePointerShape(
            frame_info.PointerShapeBufferSize,
            cursor_bitmap_.data(),
            &required_size,
            &cursor_info_
        );
    }

    // 更新光标位置
    cursor_position_ = {frame_info.PointerPosition.Position.x, 
                        frame_info.PointerPosition.Position.y};
    cursor_visible_ = frame_info.PointerPosition.Visible != 0;

    if (!cursor_visible_ || cursor_bitmap_.empty()) return;

    // 软件合成光标到BGRA帧
    // 注意：这里简化处理，实际需支持不同光标类型（单色、彩色、遮罩）
    const int cursor_x = cursor_position_.x;
    const int cursor_y = cursor_position_.y;
    const int cursor_w = cursor_info_.Width;
    const int cursor_h = cursor_info_.Height;

    for (int y = 0; y < cursor_h && (cursor_y + y) < static_cast<int>(output_height_); ++y) {
        for (int x = 0; x < cursor_w && (cursor_x + x) < static_cast<int>(output_width_); ++x) {
            // 简化：假设彩色光标，直接覆盖
            // 实际需根据cursor_info.Type处理（DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR等）
            int frame_offset = ((cursor_y + y) * pitch) + ((cursor_x + x) * 4);
            int cursor_offset = (y * cursor_w + x) * 4;
            
            if (frame_offset >= 0 && cursor_offset + 3 < cursor_bitmap_.size()) {
                // Alpha混合（简化版）
                uint8_t* pixel = frame_data + frame_offset;
                uint8_t* cursor_pixel = cursor_bitmap_.data() + cursor_offset;
                // ... 混合逻辑
            }
        }
    }
}

} // namespace duke

#endif // DUKE_PLATFORM_WINDOWS
```

---

### 2.3 编码模块

**`src/encode/encode_engine.h`** —— 编码引擎接口

```cpp
#pragma once
#include "../core/platform.h"

namespace duke {

class EncodeEngine {
public:
    virtual ~EncodeEngine() = default;
    
    virtual bool Initialize(uint32_t width, uint32_t height, uint32_t fps, 
                           uint32_t bitrate_kbps) = 0;
    virtual bool EncodeFrame(const uint8_t* data, const FrameDesc& desc,
                             std::vector<EncodedFrame>& output) = 0;
    virtual bool ForceKeyframe() = 0;  // 强制IDR（场景切换）
    virtual bool Drain(std::vector<EncodedFrame>& output) = 0;
    virtual void SetBitrate(uint32_t bitrate_kbps) = 0;  // 动态码率
};

// 工厂
std::unique_ptr<EncodeEngine> CreateHardwareEncoder();
std::unique_ptr<EncodeEngine> CreateX264Encoder();

} // namespace duke
```

**`src/encode/x264_encoder.h/.cpp`** —— x264软件编码（跨平台fallback）

```cpp
// x264_encoder.h
#pragma once
#include "encode_engine.h"

extern "C" {
#include <x264.h>
}

namespace duke {

class X264Encoder : public EncodeEngine {
public:
    X264Encoder();
    ~X264Encoder() override;

    bool Initialize(uint32_t width, uint32_t height, uint32_t fps,
                   uint32_t bitrate_kbps) override;
    bool EncodeFrame(const uint8_t* data, const FrameDesc& desc,
                     std::vector<EncodedFrame>& output) override;
    bool ForceKeyframe() override;
    bool Drain(std::vector<EncodedFrame>& output) override;
    void SetBitrate(uint32_t bitrate_kbps) override;

private:
    bool EncodeInternal(x264_picture_t* pic, std::vector<EncodedFrame>& output);
    
    x264_t* encoder_ = nullptr;
    x264_param_t param_{};
    x264_picture_t pic_in_{};
    x264_picture_t pic_out_{};
    
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t fps_ = 0;
    int64_t pts_counter_ = 0;
    bool force_keyframe_ = false;
};

} // namespace duke
```

```cpp
// x264_encoder.cpp
#include "x264_encoder.h"
#include <spdlog/spdlog.h>

namespace duke {

X264Encoder::X264Encoder() {
    x264_picture_init(&pic_in_);
    x264_picture_init(&pic_out_);
}

X264Encoder::~X264Encoder() {
    if (encoder_) {
        x264_encoder_close(encoder_);
    }
    x264_picture_clean(&pic_in_);
}

bool X264Encoder::Initialize(uint32_t width, uint32_t height, uint32_t fps,
                            uint32_t bitrate_kbps) {
    width_ = width;
    height_ = height;
    fps_ = fps;

    // 预设：屏幕内容优化
    if (x264_param_default_preset(&param_, "medium", "zerolatency") < 0) {
        return false;
    }

    // 屏幕内容专用参数
    param_.i_width = width;
    param_.i_height = height;
    param_.i_fps_num = fps;
    param_.i_fps_den = 1;
    param_.i_keyint_max = fps * 2;  // 2秒一个IDR
    param_.i_keyint_min = fps;      // 最小1秒
    param_.b_intra_refresh = 0;     // 屏幕内容禁用intra refresh（影响随机访问）
    param_.rc.i_rc_method = X264_RC_ABR;
    param_.rc.i_bitrate = bitrate_kbps;
    param_.rc.i_vbv_max_bitrate = bitrate_kbps * 1.5;
    param_.rc.i_vbv_buffer_size = bitrate_kbps;  // 1秒缓冲
    
    // 屏幕内容优化工具
    param_.analyse.b_ssim = 0;
    param_.analyse.b_psnr = 0;
    param_.i_threads = 4;  // 多线程编码
    
    // 禁用B帧（降低延迟，简化时间戳）
    param_.i_bframe = 0;
    param_.b_open_gop = 0;
    
    // 颜色空间
    param_.i_csp = X264_CSP_NV12;

    // 应用参数
    x264_param_apply_profile(&param_, "high");

    encoder_ = x264_encoder_open(&param_);
    if (!encoder_) {
        spdlog::error("Failed to open x264 encoder");
        return false;
    }

    // 分配输入picture
    x264_picture_alloc(&pic_in_, X264_CSP_NV12, width, height);
    
    spdlog::info("x264 encoder initialized: {}x{}@{} {}kbps", 
                 width, height, fps, bitrate_kbps);
    return true;
}

bool X264Encoder::EncodeFrame(const uint8_t* data, const FrameDesc& desc,
                             std::vector<EncodedFrame>& output) {
    if (!encoder_) return false;

    // 假设输入是NV12格式（从DXGI BGRA转换而来，实际需用libyuv转换）
    // 这里简化，假设data已经是NV12
    pic_in_.img.plane[0] = const_cast<uint8_t*>(data);  // Y
    pic_in_.img.plane[1] = const_cast<uint8_t*>(data + width_ * height_);  // UV
    
    pic_in_.i_pts = pts_counter_++;
    pic_in_.i_type = force_keyframe_ ? X264_TYPE_IDR : X264_TYPE_AUTO;
    force_keyframe_ = false;

    return EncodeInternal(&pic_in_, output);
}

bool X264Encoder::ForceKeyframe() {
    force_keyframe_ = true;
    return true;
}

bool X264Encoder::Drain(std::vector<EncodedFrame>& output) {
    return EncodeInternal(nullptr, output);  // pic = nullptr 表示flush
}

void X264Encoder::SetBitrate(uint32_t bitrate_kbps) {
    if (!encoder_) return;
    param_.rc.i_bitrate = bitrate_kbps;
    x264_encoder_reconfig(encoder_, &param_);
}

bool X264Encoder::EncodeInternal(x264_picture_t* pic, 
                                 std::vector<EncodedFrame>& output) {
    x264_nal_t* nals = nullptr;
    int i_nal = 0;

    int frame_size = x264_encoder_encode(encoder_, &nals, &i_nal, pic, &pic_out_);
    if (frame_size < 0) {
        spdlog::error("x264 encode failed");
        return false;
    }

    if (frame_size == 0) {
        return true;  // 缓冲中，无输出
    }

    // 收集NAL单元
    EncodedFrame frame;
    frame.data.reserve(frame_size);
    frame.pts = pic_out_.i_pts;
    frame.dts = pic_out_.i_dts;
    frame.desc.is_keyframe = (pic_out_.i_type == X264_TYPE_IDR || 
                              pic_out_.i_type == X264_TYPE_I);
    
    for (int i = 0; i < i_nal; ++i) {
        frame.data.insert(frame.data.end(), 
                         nals[i].p_payload, 
                         nals[i].p_payload + nals[i].i_payload);
    }
    
    frame.desc.width = width_;
    frame.desc.height = height_;
    frame.desc.timestamp_us = NowUs();
    
    output.push_back(std::move(frame));
    return true;
}

} // namespace duke
```

---

### 2.4 fMP4 流式封装模块

**`src/mux/fmp4_muxer.h/.cpp`** —— 核心中的核心，流式分片封装

```cpp
// fmp4_muxer.h
#pragma once
#include "../core/platform.h"
#include <fstream>

namespace duke {

// fMP4分片信息
struct FragmentInfo {
    int64_t sequence_number = 0;
    int64_t start_pts = 0;
    int64_t end_pts = 0;
    int64_t duration_ms = 0;
    size_t file_offset = 0;
    size_t size = 0;
    bool independent = false;  // 是否以IDR开头
};

// fMP4 Muxer配置
struct MuxerConfig {
    uint32_t timescale = 90000;  // 媒体时间戳精度
    uint32_t video_track_id = 1;
    uint32_t fragment_duration_ms = 4000;  // 4秒分片
};

class Fmp4Muxer {
public:
    explicit Fmp4Muxer(const MuxerConfig& config = {});
    ~Fmp4Muxer();

    // 初始化输出文件
    bool Initialize(const std::string& output_path, uint32_t width, uint32_t height);
    
    // 写入编码帧
    bool WriteFrame(const EncodedFrame& frame);
    
    // 强制分片（场景切换时）
    bool ForceFragment();
    
    // 关闭并写入尾部
    bool Finalize();
    
    // 获取已完成的fragment列表（用于上传）
    std::vector<FragmentInfo> GetCompletedFragments();
    
    // 流式模式：获取当前可读取的数据偏移
    bool GetStreamOffset(size_t& offset, size_t& available_size);

private:
    // Box写入工具
    void WriteBoxHeader(const char* type, uint32_t size);
    void WriteFullBoxHeader(const char* type, uint32_t size, uint8_t version, uint32_t flags);
    void WriteUint32(uint32_t val);
    void WriteUint64(uint64_t val);
    void WriteBytes(const uint8_t* data, size_t len);
    
    // 关键Box生成
    bool WriteFtyp();   // 文件类型
    bool WriteMoov();   // 电影头（前置）
    bool WriteMoof(int64_t sequence_number, const EncodedFrame& first_frame);
    bool WriteMdat(const std::vector<EncodedFrame>& frames);
    bool WriteMfra();   // 随机访问索引（可选）
    
    // 分片管理
    bool FlushFragment();
    
    MuxerConfig config_;
    std::string output_path_;
    std::fstream file_;
    
    // 视频参数
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t sps_[64] = {};
    uint32_t pps_[64] = {};
    size_t sps_len_ = 0;
    size_t pps_len_ = 0;
    
    // 当前分片缓冲
    std::vector<EncodedFrame> current_fragment_;
    int64_t fragment_sequence_ = 0;
    int64_t base_media_decode_time_ = 0;
    
    // 文件偏移追踪
    size_t file_offset_ = 0;
    size_t moov_offset_ = 0;
    size_t moov_size_ = 0;
    
    // 已完成的fragment
    std::vector<FragmentInfo> completed_fragments_;
    std::mutex fragment_mutex_;
    
    // SPS/PPS提取状态
    bool has_extradata_ = false;
};

} // namespace duke
```

```cpp
// fmp4_muxer.cpp
#include "fmp4_muxer.h"
#include <spdlog/spdlog.h>
#include <cstring>

namespace duke {

// Box类型宏
#define BOX_TYPE(a,b,c,d) ((a)<<24 | (b)<<16 | (c)<<8 | (d))

Fmp4Muxer::Fmp4Muxer(const MuxerConfig& config) : config_(config) {}
Fmp4Muxer::~Fmp4Muxer() {
    if (file_.is_open()) {
        Finalize();
    }
}

bool Fmp4Muxer::Initialize(const std::string& output_path, uint32_t width, uint32_t height) {
    output_path_ = output_path;
    width_ = width;
    height_ = height;
    
    file_.open(output_path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file_.is_open()) {
        spdlog::error("Failed to open output file: {}", output_path);
        return false;
    }
    
    // 预分配文件空间（避免碎片化）
#ifdef DUKE_PLATFORM_WINDOWS
    // Windows: SetFileValidData 或 SetEndOfFile
    HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file_.)));
    LARGE_INTEGER size;
    size.QuadPart = 10LL * 1024 * 1024 * 1024;  // 预分配10GB
    SetFilePointerEx(hFile, size, nullptr, FILE_BEGIN);
    SetEndOfFile(hFile);
    SetFilePointerEx(hFile, {0}, nullptr, FILE_BEGIN);
#else
    // macOS/Linux: ftruncate
    ftruncate(fileno(file_), 10LL * 1024 * 1024 * 1024);
    fseeko(file_, 0, SEEK_SET);
#endif

    // 写入ftyp
    if (!WriteFtyp()) return false;
    
    // 预留moov空间（前置）
    moov_offset_ = file_offset_;
    // moov大小未知，先写入占位，最后回填
    WriteBoxHeader("moov", 0);  // 占位，size=0表示未确定
    
    spdlog::info("fMP4 muxer initialized: {}", output_path);
    return true;
}

bool Fmp4Muxer::WriteFrame(const EncodedFrame& frame) {
    if (!file_.is_open()) return false;

    // 提取SPS/PPS（H.264）
    if (!has_extradata_) {
        ExtractExtradata(frame.data.data(), frame.data.size());
    }

    // 检查是否需要分片
    bool should_fragment = false;
    if (!current_fragment_.empty()) {
        int64_t fragment_duration = frame.pts - current_fragment_.front().pts;
        if (fragment_duration >= config_.fragment_duration_ms * 90) {  // 90kHz timescale
            should_fragment = true;
        }
    }

    if (should_fragment && frame.desc.is_keyframe) {
        // 只有IDR帧才能作为新分片起点
        if (!FlushFragment()) return false;
    }

    current_fragment_.push_back(frame);
    
    // 如果是第一帧，记录base_media_decode_time
    if (current_fragment_.size() == 1) {
        base_media_decode_time_ = frame.dts;
    }

    return true;
}

bool Fmp4Muxer::ForceFragment() {
    return FlushFragment();
}

bool Fmp4Muxer::FlushFragment() {
    if (current_fragment_.empty()) return true;

    // 构建moof + mdat
    int64_t seq = fragment_sequence_++;
    size_t fragment_start = file_offset_;
    
    // moof
    if (!WriteMoof(seq, current_fragment_.front())) return false;
    
    // mdat
    if (!WriteMdat(current_fragment_)) return false;
    
    // 记录fragment信息
    FragmentInfo info;
    info.sequence_number = seq;
    info.start_pts = current_fragment_.front().pts;
    info.end_pts = current_fragment_.back().pts;
    info.duration_ms = (info.end_pts - info.start_pts) / 90;
    info.file_offset = fragment_start;
    info.size = file_offset_ - fragment_start;
    info.independent = current_fragment_.front().desc.is_keyframe;
    
    {
        std::lock_guard<std::mutex> lock(fragment_mutex_);
        completed_fragments_.push_back(info);
    }
    
    spdlog::debug("Fragment {} flushed: {} frames, {} ms, {} bytes", 
                  seq, current_fragment_.size(), info.duration_ms, info.size);
    
    current_fragment_.clear();
    return true;
}

bool Fmp4Muxer::Finalize() {
    if (!file_.is_open()) return false;
    
    // 刷新最后一个分片
    FlushFragment();
    
    // 回到moov位置，写入真实moov
    size_t current_pos = file_offset_;
    file_.seekp(moov_offset_);
    file_offset_ = moov_offset_;
    
    if (!WriteMoov()) {
        spdlog::error("Failed to write moov");
        return false;
    }
    moov_size_ = file_offset_ - moov_offset_;
    
    // 截断文件到实际大小
    file_.seekp(0, std::ios::end);
    // ... 截断逻辑
    
    file_.close();
    spdlog::info("fMP4 finalized: {} fragments, {} bytes", 
                 completed_fragments_.size(), file_offset_);
    return true;
}

std::vector<FragmentInfo> Fmp4Muxer::GetCompletedFragments() {
    std::lock_guard<std::mutex> lock(fragment_mutex_);
    return completed_fragments_;
}

bool Fmp4Muxer::GetStreamOffset(size_t& offset, size_t& available_size) {
    std::lock_guard<std::mutex> lock(fragment_mutex_);
    if (completed_fragments_.empty()) {
        offset = 0;
        available_size = 0;
        return false;
    }
    auto& last = completed_fragments_.back();
    offset = last.file_offset + last.size;
    available_size = file_offset_ - offset;
    return true;
}

// ========== Box写入工具 ==========

void Fmp4Muxer::WriteBoxHeader(const char* type, uint32_t size) {
    if (size == 0) {
        // 扩展大小（64位），先占位
        WriteUint32(1);  // size=1表示扩展
        file_.write(type, 4);
        WriteUint64(0);  // 64位大小占位
    } else {
        WriteUint32(size);
        file_.write(type, 4);
    }
    file_offset_ += 8;
}

void Fmp4Muxer::WriteFullBoxHeader(const char* type, uint32_t size, 
                                   uint8_t version, uint32_t flags) {
    WriteBoxHeader(type, size);
    uint8_t vf[4] = {version, static_cast<uint8_t>(flags >> 16), 
                     static_cast<uint8_t>(flags >> 8), static_cast<uint8_t>(flags)};
    file_.write(reinterpret_cast<char*>(vf), 4);
    file_offset_ += 4;
}

void Fmp4Muxer::WriteUint32(uint32_t val) {
    uint8_t bytes[4] = {
        static_cast<uint8_t>(val >> 24),
        static_cast<uint8_t>(val >> 16),
        static_cast<uint8_t>(val >> 8),
        static_cast<uint8_t>(val)
    };
    file_.write(reinterpret_cast<char*>(bytes), 4);
    file_offset_ += 4;
}

void Fmp4Muxer::WriteUint64(uint64_t val) {
    uint8_t bytes[8];
    for (int i = 0; i < 8; ++i) {
        bytes[i] = static_cast<uint8_t>(val >> (56 - i * 8));
    }
    file_.write(reinterpret_cast<char*>(bytes), 8);
    file_offset_ += 8;
}

void Fmp4Muxer::WriteBytes(const uint8_t* data, size_t len) {
    file_.write(reinterpret_cast<const char*>(data), len);
    file_offset_ += len;
}

// ========== 关键Box生成 ==========

bool Fmp4Muxer::WriteFtyp() {
    // ftyp box: major_brand + minor_version + compatible_brands
    const char* brands = "isommp42msdhmsix";
    uint32_t size = 8 + 4 + 4 + strlen(brands);
    WriteBoxHeader("ftyp", size);
    file_.write("isom", 4);
    WriteUint32(0x200);  // minor version
    file_.write(brands, strlen(brands));
    file_offset_ += 4 + 4 + strlen(brands);
    return true;
}

bool Fmp4Muxer::WriteMoov() {
    // moov包含：mvhd + trak (tkhd + mdia + minf + stbl) + mvex (mehd + trex)
    // 简化版，实际需完整实现
    size_t moov_start = file_offset_;
    
    // 先计算大小，或写入占位后回填
    // 这里简化处理，实际生产需精确计算
    WriteBoxHeader("moov", 0);  // 扩展大小
    
    // mvhd (movie header)
    // ... 省略具体实现
    
    // trak (video track)
    // ... 
    
    // mvex (movie extends)
    // ...
    
    return true;
}

bool Fmp4Muxer::WriteMoof(int64_t sequence_number, const EncodedFrame& first_frame) {
    // moof = mfhd + traf (tfhd + tfdt + trun)
    size_t moof_start = file_offset_;
    
    // 计算moof大小（需提前知道trun的sample count）
    size_t sample_count = current_fragment_.size();
    
    // 简化：直接写入
    // 实际需完整实现所有box
    
    return true;
}

bool Fmp4Muxer::WriteMdat(const std::vector<EncodedFrame>& frames) {
    // 计算总大小
    size_t total_size = 0;
    for (const auto& f : frames) {
        total_size += f.data.size();
    }
    
    WriteBoxHeader("mdat", static_cast<uint32_t>(total_size + 8));
    for (const auto& f : frames) {
        WriteBytes(f.data.data(), f.data.size());
    }
    return true;
}

// ========== 辅助函数 ==========

void Fmp4Muxer::ExtractExtradata(const uint8_t* data, size_t size) {
    // 从H.264码流中提取SPS/PPS
    size_t i = 0;
    while (i + 4 < size) {
        // 查找NAL起始码
        size_t start = 0;
        if (data[i] == 0 && data[i+1] == 0 && data[i+2] == 0 && data[i+3] == 1) {
            start = i + 4;
        } else if (data[i] == 0 && data[i+1] == 0 && data[i+2] == 1) {
            start = i + 3;
        } else {
            i++;
            continue;
        }
        
        uint8_t nal_type = data[start] & 0x1F;
        if (nal_type == 7) {  // SPS
            // 找到下一个NAL起始码
            size_t end = start + 1;
            while (end + 4 < size) {
                if ((data[end] == 0 && data[end+1] == 0 && data[end+2] == 0 && data[end+3] == 1) ||
                    (data[end] == 0 && data[end+1] == 0 && data[end+2] == 1)) {
                    break;
                }
                end++;
            }
            sps_len_ = std::min(end - start, size_t(64));
            memcpy(sps_, data + start, sps_len_);
        } else if (nal_type == 8) {  // PPS
            // 类似提取
        }
        
        i = start;
    }
    
    has_extradata_ = (sps_len_ > 0);
}

} // namespace duke
```

---

### 2.5 IPC模块（零拷贝跨端通信）

**`src/ipc/shared_memory.h/.cpp`** —— 共享内存实现

```cpp
// shared_memory.h
#pragma once
#include "../core/platform.h"

namespace duke {

// 共享内存区域描述
struct SharedMemoryRegion {
    static constexpr size_t kMaxFrames = 8;
    static constexpr size_t kFrameSize = 8 * 1024 * 1024;  // 8MB per frame
    
    struct FrameSlot {
        std::atomic<bool> ready{false};
        std::atomic<bool> consumed{false};
        uint32_t data_size = 0;
        FrameDesc desc;
        uint8_t data[kFrameSize];
    };
    
    std::atomic<uint32_t> write_index{0};
    std::atomic<uint32_t> read_index{0};
    FrameSlot slots[kMaxFrames];
};

class SharedMemory {
public:
    SharedMemory();
    ~SharedMemory();
    
    // 创建/打开共享内存
    bool Create(const std::string& name, size_t size);
    bool Open(const std::string& name);
    
    // 映射到当前进程
    bool Map();
    
    // 获取底层指针
    void* GetBase() const { return base_; }
    size_t GetSize() const { return size_; }
    
    // 视频帧专用API
    bool WriteFrame(const EncodedFrame& frame);
    bool ReadFrame(EncodedFrame& frame);
    
private:
    std::string name_;
    size_t size_ = 0;
    void* base_ = nullptr;
    
#ifdef DUKE_PLATFORM_WINDOWS
    HANDLE handle_ = nullptr;
#else
    int fd_ = -1;
#endif
};

} // namespace duke
```

```cpp
// shared_memory.cpp
#include "shared_memory.h"
#include <spdlog/spdlog.h>

#ifdef DUKE_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace duke {

SharedMemory::SharedMemory() = default;
SharedMemory::~SharedMemory() {
    if (base_) {
#ifdef DUKE_PLATFORM_WINDOWS
        UnmapViewOfFile(base_);
#else
        munmap(base_, size_);
#endif
    }
#ifdef DUKE_PLATFORM_WINDOWS
    if (handle_) CloseHandle(handle_);
#else
    if (fd_ >= 0) close(fd_);
#endif
}

bool SharedMemory::Create(const std::string& name, size_t size) {
    name_ = name;
    size_ = size;
    
#ifdef DUKE_PLATFORM_WINDOWS
    handle_ = CreateFileMappingA(
        INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
        0, static_cast<DWORD>(size), name.c_str()
    );
    if (!handle_) {
        spdlog::error("CreateFileMapping failed: {}", GetLastError());
        return false;
    }
#else
    fd_ = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd_ < 0) {
        spdlog::error("shm_open failed: {}", errno);
        return false;
    }
    ftruncate(fd_, size);
#endif

    return Map();
}

bool SharedMemory::Open(const std::string& name) {
    name_ = name;
    
#ifdef DUKE_PLATFORM_WINDOWS
    handle_ = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
    if (!handle_) return false;
    
    // 获取大小
    // Windows需通过其他方式传递大小，这里简化
    size_ = sizeof(SharedMemoryRegion);
#else
    fd_ = shm_open(name.c_str(), O_RDWR, 0666);
    if (fd_ < 0) return false;
    
    struct stat st;
    fstat(fd_, &st);
    size_ = st.st_size;
#endif

    return Map();
}

bool SharedMemory::Map() {
#ifdef DUKE_PLATFORM_WINDOWS
    base_ = MapViewOfFile(handle_, FILE_MAP_ALL_ACCESS, 0, 0, size_);
#else
    base_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (base_ == MAP_FAILED) base_ = nullptr;
#endif

    if (!base_) {
        spdlog::error("Memory mapping failed");
        return false;
    }
    return true;
}

bool SharedMemory::WriteFrame(const EncodedFrame& frame) {
    auto* region = static_cast<SharedMemoryRegion*>(base_);
    if (!region) return false;
    
    if (frame.data.size() > SharedMemoryRegion::kFrameSize) {
        spdlog::error("Frame too large: {} > {}", frame.data.size(), 
                      SharedMemoryRegion::kFrameSize);
        return false;
    }
    
    // 找到可写入的slot
    uint32_t idx = region->write_index.load(std::memory_order_relaxed) % 
                   SharedMemoryRegion::kMaxFrames;
    int attempts = 0;
    while (region->slots[idx].ready.load(std::memory_order_acquire)) {
        idx = (idx + 1) % SharedMemoryRegion::kMaxFrames;
        if (++attempts >= SharedMemoryRegion::kMaxFrames) {
            spdlog::warn("All shared memory slots full, dropping frame");
            return false;
        }
    }
    
    auto& slot = region->slots[idx];
    slot.desc = frame.desc;
    slot.data_size = static_cast<uint32_t>(frame.data.size());
    memcpy(slot.data, frame.data.data(), frame.data.size());
    slot.consumed.store(false, std::memory_order_relaxed);
    slot.ready.store(true, std::memory_order_release);
    
    region->write_index.store(idx + 1, std::memory_order_release);
    return true;
}

bool SharedMemory::ReadFrame(EncodedFrame& frame) {
    auto* region = static_cast<SharedMemoryRegion*>(base_);
    if (!region) return false;
    
    uint32_t idx = region->read_index.load(std::memory_order_relaxed) % 
                   SharedMemoryRegion::kMaxFrames;
    
    auto& slot = region->slots[idx];
    if (!slot.ready.load(std::memory_order_acquire)) {
        return false;  // 无数据
    }
    
    frame.desc = slot.desc;
    frame.data.assign(slot.data, slot.data + slot.data_size);
    
    slot.ready.store(false, std::memory_order_release);
    region->read_index.store(idx + 1, std::memory_order_release);
    return true;
}

} // namespace duke
```

---

### 2.6 主控引擎与流水线

**`src/recorder_engine.h/.cpp`** —— 总控，串联采集→编码→封装→上传

```cpp
// recorder_engine.h
#pragma once
#include "core/platform.h"
#include "capture/capture_engine.h"
#include "encode/encode_engine.h"
#include "mux/fmp4_muxer.h"
#include "ipc/shared_memory.h"

namespace duke {

enum class RecorderState {
    Idle,
    Initializing,
    Recording,
    Paused,
    Stopping,
    Error
};

class RecorderEngine {
public:
    RecorderEngine();
    ~RecorderEngine();

    bool Initialize(const RecorderConfig& config);
    bool Start();
    bool Pause();   // 暂停（保持编码器状态）
    bool Resume();  // 恢复（时间戳连续）
    bool Stop();
    
    RecorderState GetState() const;
    std::string GetError() const;
    
    // 流式上传回调
    using UploadCallback = std::function<void(const FragmentInfo& fragment)>;
    void SetUploadCallback(UploadCallback cb);

private:
    void CaptureLoop();   // 采集线程
    void EncodeLoop();    // 编码线程
    void MuxLoop();       // 封装线程
    void UploadLoop();    // 上传线程
    
    bool SetupPipeline();
    void TeardownPipeline();
    
    RecorderConfig config_;
    RecorderState state_ = RecorderState::Idle;
    std::string last_error_;
    
    // 模块
    std::unique_ptr<CaptureEngine> capture_;
    std::unique_ptr<EncodeEngine> encoder_;
    std::unique_ptr<Fmp4Muxer> muxer_;
    std::unique_ptr<SharedMemory> ipc_shm_;
    
    // 线程
    std::thread capture_thread_;
    std::thread encode_thread_;
    std::thread mux_thread_;
    std::thread upload_thread_;
    std::atomic<bool> running_{false};
    
    // 线程间通信
    FrameBuffer frame_buffer_{8, 10 * 1024 * 1024};  // 8 slot, 10MB max
    std::mutex encode_queue_mutex_;
    std::condition_variable encode_cv_;
    std::vector<EncodedFrame> encode_queue_;
    
    std::mutex mux_queue_mutex_;
    std::condition_variable mux_cv_;
    std::vector<EncodedFrame> mux_queue_;
    
    // 暂停状态
    std::atomic<bool> paused_{false};
    int64_t pause_pts_ = 0;
    int64_t total_pause_duration_ = 0;  // 累计暂停时间（用于时间戳补偿）
    
    // 上传
    UploadCallback upload_cb_;
    std::mutex fragment_mutex_;
    std::vector<FragmentInfo> pending_fragments_;
    
    // 统计
    struct Stats {
        std::atomic<uint64_t> frames_captured{0};
        std::atomic<uint64_t> frames_encoded{0};
        std::atomic<uint64_t> fragments_muxed{0};
        std::atomic<uint64_t> bytes_uploaded{0};
        std::atomic<uint64_t> dropped_frames{0};
    } stats_;
};

} // namespace duke
```

```cpp
// recorder_engine.cpp
#include "recorder_engine.h"
#include <spdlog/spdlog.h>

namespace duke {

RecorderEngine::RecorderEngine() = default;
RecorderEngine::~RecorderEngine() {
    Stop();
}

bool RecorderEngine::Initialize(const RecorderConfig& config) {
    config_ = config;
    state_ = RecorderState::Initializing;
    
    // 创建采集引擎
    capture_ = CreateCaptureEngine();
    if (!capture_->Initialize(config)) {
        last_error_ = "Capture initialization failed";
        state_ = RecorderState::Error;
        return false;
    }
    
    // 获取实际采集参数
    uint32_t cap_w, cap_h, cap_fps;
    capture_->GetEncoderParams(cap_w, cap_h, cap_fps);
    
    // 创建编码引擎（优先硬件）
    encoder_ = CreateHardwareEncoder();
    if (!encoder_ || !encoder_->Initialize(cap_w, cap_h, cap_fps, config.bitrate_kbps)) {
        spdlog::warn("Hardware encoder failed, falling back to x264");
        encoder_ = CreateX264Encoder();
        if (!encoder_->Initialize(cap_w, cap_h, cap_fps, config.bitrate_kbps)) {
            last_error_ = "Encoder initialization failed";
            state_ = RecorderState::Error;
            return false;
        }
    }
    
    // 创建封装器
    muxer_ = std::make_unique<Fmp4Muxer>();
    if (!muxer_->Initialize(config.output_path, cap_w, cap_h)) {
        last_error_ = "Muxer initialization failed";
        state_ = RecorderState::Error;
        return false;
    }
    
    // 创建IPC共享内存（用于Electron通信）
    if (config.enable_streaming) {
        ipc_shm_ = std::make_unique<SharedMemory>();
        if (!ipc_shm_->Create("DukeRecorder_SharedMem", sizeof(SharedMemoryRegion))) {
            spdlog::warn("Shared memory creation failed, IPC disabled");
            ipc_shm_.reset();
        }
    }
    
    state_ = RecorderState::Idle;
    spdlog::info("RecorderEngine initialized: {}x{}@{} {}kbps", 
                 cap_w, cap_h, cap_fps, config.bitrate_kbps);
    return true;
}

bool RecorderEngine::Start() {
    if (state_ != RecorderState::Idle && state_ != RecorderState::Paused) {
        return false;
    }
    
    running_ = true;
    state_ = RecorderState::Recording;
    
    // 启动流水线线程
    capture_thread_ = std::thread(&RecorderEngine::CaptureLoop, this);
    encode_thread_ = std::thread(&RecorderEngine::EncodeLoop, this);
    mux_thread_ = std::thread(&RecorderEngine::MuxLoop, this);
    
    if (config_.enable_streaming) {
        upload_thread_ = std::thread(&RecorderEngine::UploadLoop, this);
    }
    
    spdlog::info("Recording started");
    return true;
}

bool RecorderEngine::Pause() {
    if (state_ != RecorderState::Recording) return false;
    
    paused_.store(true);
    pause_pts_ = NowUs();  // 记录暂停时间点
    state_ = RecorderState::Paused;
    
    spdlog::info("Recording paused");
    return true;
}

bool RecorderEngine::Resume() {
    if (state_ != RecorderState::Paused) return false;
    
    // 计算暂停持续时间，用于时间戳补偿
    int64_t pause_duration = NowUs() - pause_pts_;
    total_pause_duration_ += pause_duration;
    
    paused_.store(false);
    
    // 强制插入IDR帧（确保恢复后可独立解码）
    encoder_->ForceKeyframe();
    
    state_ = RecorderState::Recording;
    spdlog::info("Recording resumed, pause duration: {} ms", pause_duration / 1000);
    return true;
}

bool RecorderEngine::Stop() {
    if (state_ != RecorderState::Recording && state_ != RecorderState::Paused) {
        return false;
    }
    
    state_ = RecorderState::Stopping;
    running_.store(false);
    
    // 唤醒所有等待线程
    encode_cv_.notify_all();
    mux_cv_.notify_all();
    
    // 等待线程结束
    if (capture_thread_.joinable()) capture_thread_.join();
    if (encode_thread_.joinable()) encode_thread_.join();
    if (mux_thread_.joinable()) mux_thread_.join();
    if (upload_thread_.joinable()) upload_thread_.join();
    
    // Finalize封装
    muxer_->Finalize();
    
    state_ = RecorderState::Idle;
    spdlog::info("Recording stopped");
    return true;
}

RecorderState RecorderEngine::GetState() const {
    return state_;
}

std::string RecorderEngine::GetError() const {
    return last_error_;
}

void RecorderEngine::SetUploadCallback(UploadCallback cb) {
    upload_cb_ = std::move(cb);
}

// ========== 流水线线程 ==========

void RecorderEngine::CaptureLoop() {
    // 设置采集回调
    capture_->SetCallback([this](const uint8_t* data, const FrameDesc& desc) {
        if (paused_.load()) return;  // 暂停时丢弃帧
        
        auto* slot = frame_buffer_.AcquireWriteSlot();
        if (!slot) {
            stats_.dropped_frames++;
            return;
        }
        
        slot->data.assign(data, data + desc.stride * desc.height);
        slot->desc = desc;
        frame_buffer_.CommitWrite(slot);
    });
    
    capture_->Start();
    
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    capture_->Stop();
}

void RecorderEngine::EncodeLoop() {
    while (running_) {
        auto* slot = frame_buffer_.AcquireReadSlot();
        if (!slot) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        
        // 时间戳补偿（扣除暂停时间）
        slot->desc.timestamp_us -= total_pause_duration_;
        
        std::vector<EncodedFrame> encoded;
        if (encoder_->EncodeFrame(slot->data.data(), slot->desc, encoded)) {
            std::lock_guard<std::mutex> lock(mux_queue_mutex_);
            for (auto& frame : encoded) {
                // 应用时间戳补偿
                frame.pts -= total_pause_duration_ * 90 / 1000;  // 转换为90kHz
                frame.dts -= total_pause_duration_ * 90 / 1000;
                mux_queue_.push_back(std::move(frame));
            }
            mux_cv_.notify_one();
        }
        
        frame_buffer_.ReleaseReadSlot(slot);
        stats_.frames_captured++;
    }
}

void RecorderEngine::MuxLoop() {
    while (running_) {
        std::vector<EncodedFrame> frames;
        {
            std::unique_lock<std::mutex> lock(mux_queue_mutex_);
            mux_cv_.wait(lock, [this] { return !mux_queue_.empty() || !running_; });
            
            if (!running_ && mux_queue_.empty()) break;
            
            frames.swap(mux_queue_);
        }
        
        for (auto& frame : frames) {
            // 写入封装
            muxer_->WriteFrame(frame);
            stats_.frames_encoded++;
            
            // IPC共享内存（供Electron预览）
            if (ipc_shm_) {
                ipc_shm_->WriteFrame(frame);
            }
        }
        
        // 检查是否有新fragment可上传
        auto fragments = muxer_->GetCompletedFragments();
        {
            std::lock_guard<std::mutex> lock(fragment_mutex_);
            for (auto& f : fragments) {
                pending_fragments_.push_back(f);
            }
        }
    }
}

void RecorderEngine::UploadLoop() {
    while (running_) {
        std::vector<FragmentInfo> fragments;
        {
            std::lock_guard<std::mutex> lock(fragment_mutex_);
            fragments.swap(pending_fragments_);
        }
        
        for (auto& frag : fragments) {
            if (upload_cb_) {
                upload_cb_(frag);
            }
            stats_.fragments_muxed++;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

} // namespace duke
```

---

### 2.7 Protobuf 控制协议

**`proto/control.proto`**

```protobuf
syntax = "proto3";

package duke;

// 录制控制命令
message ControlCommand {
    enum Type {
        START = 0;
        STOP = 1;
        PAUSE = 2;
        RESUME = 3;
        CONFIGURE = 4;
    }
    Type type = 1;
    
    // CONFIGURE时携带
    RecorderConfig config = 2;
}

message RecorderConfig {
    uint32 width = 1;
    uint32 height = 2;
    uint32 fps = 3;
    uint32 bitrate_kbps = 4;
    string output_path = 5;
    bool enable_streaming = 6;
    string upload_endpoint = 7;
}

// 状态上报
message StatusReport {
    enum State {
        IDLE = 0;
        RECORDING = 1;
        PAUSED = 2;
        ERROR = 3;
    }
    State state = 1;
    uint64 duration_ms = 2;       // 录制时长
    uint64 file_size = 3;         // 文件大小
    uint32 fps = 4;               // 实际帧率
    uint32 bitrate_kbps = 5;      // 实际码率
    uint64 dropped_frames = 6;    // 丢帧数
    string error_message = 7;
}

// 流式数据通知（通过共享内存传输实际数据）
message StreamNotification {
    uint64 sequence_number = 1;
    uint64 timestamp_us = 2;
    uint32 data_size = 3;         // 共享内存中的数据大小
    bool is_keyframe = 4;
}
```

---

### 2.8 构建系统

**`CMakeLists.txt`**

```cmake
cmake_minimum_required(VERSION 3.20)
project(DukeRecorder VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 平台检测
if(WIN32)
    add_definitions(-DDUKE_PLATFORM_WINDOWS=1)
    set(PLATFORM_LIBS d3d11 dxgi dxguid mfplat mfuuid wmcodecdspuuid)
elseif(APPLE)
    add_definitions(-DDUKE_PLATFORM_MACOS=1)
    find_library(COREVIDEO CoreVideo)
    find_library(COREMEDIA CoreMedia)
    find_library(VIDEOTOOLBOX VideoToolbox)
    set(PLATFORM_LIBS ${COREVIDEO} ${COREMEDIA} ${VIDEOTOOLBOX})
endif()

# 依赖
find_package(spdlog CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

# 核心库
add_library(duke_core STATIC
    src/core/platform.h
    src/core/ring_buffer.h
    src/core/thread_pool.cpp
    src/core/timer.cpp
)

# 采集模块
add_library(duke_capture STATIC
    src/capture/capture_engine.h
    src/capture/dxgi_capture.cpp
    src/capture/graphics_capture.cpp
    src/capture/sck_capture.mm
)

# 编码模块
add_library(duke_encode STATIC
    src/encode/encode_engine.h
    src/encode/x264_encoder.cpp
    src/encode/mf_encoder.cpp
    src/encode/vt_encoder.mm
)

# 封装模块
add_library(duke_mux STATIC
    src/mux/fmp4_muxer.cpp
    src/mux/mp4_parser.cpp
    src/mux/atom_writer.cpp
)

# IPC模块
add_library(duke_ipc STATIC
    src/ipc/shared_memory.cpp
    src/ipc/named_pipe.cpp
    src/ipc/unix_socket.cpp
)

# Protobuf
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/control.proto)

# 主程序
add_executable(duke_recorder
    src/main.cpp
    src/recorder_engine.cpp
    ${PROTO_SRCS}
)

target_link_libraries(duke_recorder
    duke_core
    duke_capture
    duke_encode
    duke_mux
    duke_ipc
    spdlog::spdlog
    protobuf::libprotobuf
    x264  # 或 find_package
    ${PLATFORM_LIBS}
)

# 测试
enable_testing()
add_subdirectory(tests)
```

---

### 2.9 主入口

**`src/main.cpp`**

```cpp
#include "recorder_engine.h"
#include <spdlog/spdlog.h>
#include <iostream>

using namespace duke;

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("DukeRecorder v1.0.0 starting...");

    RecorderConfig config;
    config.width = 1920;
    config.height = 1080;
    config.fps = 30;
    config.bitrate_kbps = 4000;
    config.output_path = "recording.mp4";
    config.enable_streaming = true;
    config.upload_endpoint = "https://upload.duke.com/v1/stream";

    RecorderEngine engine;
    
    if (!engine.Initialize(config)) {
        spdlog::error("Initialization failed: {}", engine.GetError());
        return 1;
    }

    // 设置上传回调
    engine.SetUploadCallback([](const FragmentInfo& frag) {
        spdlog::info("Fragment ready for upload: seq={}, size={}, offset={}",
                     frag.sequence_number, frag.size, frag.file_offset);
        // 这里调用HTTP/2客户端实际上传
    });

    // 启动录制
    if (!engine.Start()) {
        spdlog::error("Failed to start recording");
        return 1;
    }

    spdlog::info("Recording started, press Enter to stop...");
    std::cin.get();

    // 停止
    engine.Stop();

    spdlog::info("Recording complete");
    return 0;
}
```

---

## 三、工程总结

### 已实现的核心能力

| 模块 | 实现要点 | 对应面试题 |
|------|---------|-----------|
| **采集** | DXGI + 光标合成、线程优先级控制 | 第6题（光标同步） |
| **编码** | x264 + 硬件编码抽象、内容自适应参数 | 第2题（弱网编码） |
| **封装** | 自研fMP4 Muxer、流式分片、moov前置 | 第4题（大体量文件） |
| **IPC** | 共享内存零拷贝、环形缓冲 | 第3题（跨端协作） |
| **控制** | 暂停/恢复、时间戳补偿、强制IDR | 第1题（长时间录制） |
| **架构** | 多线程流水线、背压感知、统计监控 | 第5/7题（测试/系统设计） |

### 待完善项（生产级）

1. **硬件编码实现**：MediaFoundation (Win) / VideoToolbox (mac) 具体实现
2. **fMP4完整Box实现**：moov/moof/traf/trun等所有Box的精确序列化
3. **MP4修复工具**：moov损坏时的NAL扫描修复
4. **Electron Addon**：Node-API绑定，共享内存映射到JS Buffer
5. **HTTP/2上传客户端**：基于nghttp2或libcurl的分片上传
6. **网络模拟测试**：tc/netem集成测试框架

### 设计哲学体现

> **"资源有界，流式可控，故障降级"**

- **有界**：环形缓冲区大小固定，共享内存槽位固定，预分配文件空间
- **流式**：每4秒一个fragment，秒级可播放，不累积内存
- **降级**：硬件编码失败→x264 fallback，共享内存失败→IPC禁用，上传失败→本地缓存

---

这个工程骨架涵盖了面试中所有技术决策的落地路径。每个模块都可以独立扩展测试，符合企业级音视频系统的**模块化、可观测、可容错**要求。