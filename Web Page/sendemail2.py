#!/usr/bin/python

import smtplib

sender = 'networking.project.test@gmail.com'
receivers = ['valerio.nori@hotmail.com']
passwd = 'Dbhx56hs_'
message = 'SILOS -- 2 -- IS EMPTY!'


smtpObj = smtplib.SMTP('smtp.gmail.com',587)
smtpObj.ehlo()
smtpObj.starttls()
smtpObj.ehlo()
#smtpObj.set_debuglevel(0)
print "1"
smtpObj.login(sender,passwd)
smtpObj.sendmail(sender, receivers, message)
smtpObj.close()
print "Successfully sent e-mail"
