FROM base/devel

RUN pacman -Syyu --noconfirm
RUN pacman -S clang python2 git --noconfirm
RUN git clone https://github.com/JasonL9000/ib.git /opt/ib
RUN chmod +x /opt/ib/ib
ENV PATH /opt/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/opt/ib
RUN ln -s "$(which python2)" /usr/bin/python
ADD . /opt/yourcss
WORKDIR /opt/yourcss
