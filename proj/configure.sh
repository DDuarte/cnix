cd dep
chmod -R 777 freetype-2.4.10
cd freetype-2.4.10
GNUMAKE=gmake ./configure
gmake
gmake install
cd ../../extra
cp *.ttf /root
cp cnix /etc/system.conf.d
cd ..
chmod 777 install.sh
chmod 777 run.sh
