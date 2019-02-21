


1、 安装openjdk-8:

```makefile
sudo add-apt-repository ppa:openjdk/ppa
sudo add-apt-repository ppa:openjdk-r/ppa
sudo apt-get update
sudo apt-get install openjdk-8-jdk

sudo chmod -R 755 /usr/lib/jvm/java-8-openjdk-amd64

sudo gedit ~/.bashrc

文件末尾添加如下描述信息：

#for openJDK8:
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export JRE_HOME=/usr/lib/jvm/java-8-openjdk-amd64/jre
export PATH=$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH
export CLASSPATH=$CLASSPATH:.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib

. ~/.bashrc

sudo update-alternatives --config java
sudo update-alternatives --config javac
选择需要使用的JDK版本。

确认一下路径：
printenv JAVA_HOME
java -version

```

