# An Online Data Service based on Shared Memory implemented with C++

用C++实现的基于共享内存的在线数据访问服务


## 流程

创建共享内存

* 服务启动时会申请一块几GB的共享内存，后面所有涉及到进程间共享的内存分配操作都在这块内存上进行

文件监控

* 通过inotify监控data目录下文件的变化，有更新时会触发一个事件

* 监控程序捕获这个事件，并在线程池中启动一个任务进行文件加载

加载流程切分

* loader会被切分为Batch和Incr两部分，每一部分作为一个单独的任务

* 服务启动时将每个loader需要的加载操作注册到文件监控系统中

* 对于只有全量数据的loader：

 - 只在数据被更新时触发

* 对应包含增量的loader：

 - 全量加载完成后，会生成一个增量加载的定时任务（增量加载周期可配置），由线程池定时触发

 - 同一文件全量和增量始终在同一线程中加载，确保不会冲突

 - 增量加载出错时，会在一定时间（可配置）后重试，也是通过定时器配置

* loader注册时可以设定文件通过公共的线程池（小文件）加载或是独享一个线程（对大文件）

共享内存容器

* 数据通过共享内存容器存储，有boost库提供，包括string、vector、set、map、shared_ptr、lock等

* 所有共享数据都由SharedStorage统一管理，loader将数据加入到共享容器中

* 共享容器对外提供一个DataContainer只读接口，客户端通过文件名取到对应文件的共享内存数据

自定义loader

* 普通文件继承自FileLoader，实现ParseFile方法

* 带增量加载的文件继承自FileIncrementalLoader，实现ParseFile、LoadIncr和MergeIncr方法

* 在LoaderFactory的CreateLoader注册新的loader

* 框架有个全局的配置文件指定需要加载哪些loader和对应的文件名，一个loader可以对应多份文件

客户端接口

* SharedMemoryVisitor：以只读方式访问共享内存服务的数据

* DataVisitor：以文件名为接口取共享内存服务中对应的格式化数据


## 环境部署

* 目录结构

```c++
[PROJECT_HOME]/shm_serving/
                           bin/
                               serving：共享内存在线服务主程序
                           conf/
                               shm_serving.conf：项目配置，需要加载的文件（对应的loader在LoaderFactory中设定）
                           data/
                               line_data：需要加载的文件都放这个目录下
                           glog/
                               日志输出
```

* 脚本

```c++
env/
    deploy.sh：部署
    run.sh：运行
```


## 例程

visitor

* 通过共享内存访问serving进程中line_data的结构化数据


## 工具

shared_memory_remover

* 如果共享内存服务意外退出，需要清理占用的内存


## 依赖库

* cmake

* gcc

* boost/protobuf/glog/gflags


================================
by Xiaojie Chen (swly@live.com)

