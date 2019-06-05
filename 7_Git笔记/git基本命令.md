


```c



1. 提交代码到 gerrit
git status/git status .
git add xxx.c/git add .
git commit -m "this is a test"
git push <远程主机名> <本地分支名>:<远程分支名>

(1) 怎么知道本地分支名/远程分支名？
运行 git branch -a ，会出现：
* master
  remotes/origin/HEAD -> origin/master
  remotes/origin/bctc
  remotes/origin/master
第一个带 *  号的为本地分支名，后面三个为远程分支名。如果想要提交到远程的 master 分支：
本地分支名为： master
远程分支名为： refs/for/master （不需要添加前缀 remotes/origin/ 。加上前缀 refs/for/ ，是因为 gerrit 的规则，避免出现错误： “! [remote rejected] master -> master (prohibited by Gerrit)”）。

(2) 怎么知道远程主机名？
方法1，通过 git 的 hint ：
只使用 git push 命令或者 git push <本地分支名>:<远程分支名>，
出现类似的： ssh://youname@10.30.49.63:95214/22CZ2597OL_YOUKKK_UIK_V009_WIFI_IIG.git ，这个就是远程主机名。

方法2，通过 git 命令：
git remote show ，显示了： origin
git remote show origin ，显示了：
获取地址：ssh://youname@10.30.49.63:95214/22CZ2597OL_YOUKKK_UIK_V009_WIFI_IIG.git
这个获取地址就是远程主机名。

(3) 最终的 git push 命令为：
git push ssh://youname@10.30.49.63:95214/22CZ2597OL_YOUKKK_UIK_V009_WIFI_IIG.git master:refs/for/master --tags
 --tags 用于推送本地新增的标签。



2. 其他命令：
git log . ：可以看到 commit-id
git checkout -f：回退所有修改
git clean -fd：删除新建的文件和文件夹
git reset --hard commit-id：回退到过去某个版本
git pull <远程主机名> <远程分支名>:<本地分支名> （跟 git push 类型）
git checkout -- xxx.c：回退某个文件的修改
git diff xxx.c：比较文件的修改点

切换分支：
git branch -a
* master
  remotes/origin/HEAD -> origin/master
  remotes/origin/master
  remotes/origin/release/v2.1.x
  remotes/origin/release/v2.2.x
  remotes/origin/release/v3.0.0

git checkout -b remotes/origin/release/v2.2.x

git branch -a
  master
* remotes/origin/release/v2.2.x
  remotes/origin/HEAD -> origin/master
  remotes/origin/master
  remotes/origin/release/v2.1.x
  remotes/origin/release/v2.2.x
  remotes/origin/release/v3.0.0



3. 使用 Github
(1) 配置用户名, 邮箱
	git config --global user.name embraceher
	git config --global user.email 1748494758@qq.com

(2) 查看配置
	git config --list

(3) 查看远程主机名
	git remote show origin

(4) 查看本地分支和远程分支
	git branch -a

(5) 提交代码到 github
	git push https://github.com/liuyeyaoye/Linux.git master:master --tag

(6) 配置 git, 只使用 git push 命令就可以提交代码
	git config --global push.default matching		git 将推送和远程同名的所有本地分支。
	git config --global push.default simple		只推送当前分支到远程关联的同名分支，即 'git push' 推送当前分支。
	一般使用 simple 模式。

(7) 按照上面的方法，使用 git push 提交代码，每次提交代码都要输入 Username 和 Password, 怎么避免这个情况？
设置远程主机名：
	git remote set-url origin https://github.com/liuyeyaoye/Linux.git
然后配置：
	git config --global credential.helper store
此时查看 ~/.gitconfig, 其中多了一行： helper = store


(8) git config 删除变量:   git config   --global --unset  name
git config --global remote.origin.url https://github.com/liuyeyaoye/Linux.git
git config --global --unset remote.origin.url

（9）git remote -v
远程主机名默认为： origin

```




