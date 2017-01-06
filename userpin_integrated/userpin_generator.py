import os
import sys
import random import randint

a = randint(1000,9999)
f = open('a.txt','w')
f.write(str(a))
f.close()

