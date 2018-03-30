import os
import sys

def getFileSize(filePath,size=0):
    fileList = {}
    for root,dirs,files in os.walk(filePath):
        for f in files:
            currentPath = os.path.join(root,f)
            fileSize = os.path.getsize(currentPath)
            fileList[currentPath] = fileSize
            size += fileSize
    print("%s total size = [%d]Kb" %(filePath,size/1024 + 1))
    return fileList


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('error param')
        sys.exit
    fileList = getFileSize(sys.argv[1].strip())
    for key,value in fileList.items():
        print("%s size =[%dKb]\n" % (key, (value) / 1024 + 1))
