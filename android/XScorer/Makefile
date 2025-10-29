PROJECT	=	xscorer

sign:
	zipalign -v -p 4 platforms/android/build/outputs/apk/android-release-unsigned.apk /tmp/$(PROJECT)-aligned.apk
	apksigner sign --ks /home/bs/Insync/SYS/etc/android-release-key.jks --out /home/bs/$(PROJECT).apk /tmp/$(PROJECT)-aligned.apk
	-rm -f /tmp/$(PROJECT)-aligned.apk
	apksigner verify /home/bs/$(PROJECT).apk

clean:
	find . -type f -name "*~" -exec rm -f {} \;
