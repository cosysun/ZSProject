#!/usr/bin/python2.7
# -!- coding:utf8 -!-
import sys, os, time
from os import system
from mechanize import Browser
from BeautifulSoup import BeautifulSoup
import re
import urllib2

ARTICLE_URL_SEACHR = "http://www.7dsw.com/modules/article/search.php"
ARTICLE_URL_TEST = "http://www.7dsw.com/book/0/75/19452.html"
DOWN_ALL = 1
DONW_CHAPTER = 0
BOOK_CACHE_DIR_NAME = "\\cache\\"

def GetSearchBook(bookTitle):
    lastTime = time.time()
    br = Browser()
    br.open(ARTICLE_URL_SEACHR)
    br.select_form(name = "articlesearch")
    br["searchkey"]=bookTitle
    resp = br.submit()
    data = resp.get_data().decode('gbk').encode('utf8')
    getDataTime = time.time() - lastTime
    
    lastTime = time.time()
    bookList = []
    try:
        # 获取更新页
        soup = BeautifulSoup(data)
        bookIndex = 0
        for i in soup.findAll('ul', {'class':'info'}):
            bookList.append(i.h2.a.get('href'))
            
            # 打印书籍信息
            print "[%d]"%bookIndex
            print "bookimgLink:%s"%i.img.get('src')
            print "bookTitle:%s"%i.h2.b.a.font.string
            print "bookLink:%s"%(i.h2.a.get('href'))
            print "bookIntorduce:%s"%i.li.string.replace(" ","").replace("\n","")
            print "==========================================================="   
            bookIndex += 1
        anlyseTime = time.time() - lastTime;
        print "搜索花费时间:%d秒, 分析数据花费时间:%d秒"%(getDataTime, anlyseTime)
        print "==========================================================="  
    except IndexError:
        lname = "UNKNOWN"
        print lname
    return bookList

#获取书籍目录
def GetBookDir(url):
    #读取html内容
    lastTime = time.time()
    response = urllib2.urlopen(url)
    html = response.read()
    response.close()
    html = html.decode('gbk').encode('utf8')
    getDataTime = time.time() - lastTime
    
    #分析html内容
    lastTime = time.time()
    bookDirSoup = BeautifulSoup(html)
    dirList = []
    bookIndex = 0
    try:
        for dirInfo in bookDirSoup.findAll('td', {'class':'ccss'}):
            if dirInfo.a :
                dictDir = {}
                dictDir['link'] = dirInfo.a.get('href')
                dictDir['Name'] = dirInfo.a.string
                dirList.append(dictDir)
                
                #打印目录信息
                print "[%d]:%s"%(bookIndex,dirInfo.a.string)
                print "------------------------------------------"
                bookIndex += 1
        anlyseTime = time.time() - lastTime;
        
        print "读取数据花费时间:%d秒, 分析数据花费时间:%d秒"%(getDataTime, anlyseTime)
        print "========================================================="
    except IndexError:
        print "UNKNOWN"
    
    return dirList

#获取书籍章节内容
def GetBookTxt(url, IsPrint=1):
    #读取html内容
    lastTime = time.time()
    response = urllib2.urlopen(url)
    html = response.read()
    response.close()
    getDataTime = time.time() - lastTime
    
    #分析html内容
    lastTime = time.time()
    bookContent = ""
    try:
        bookSoup = BeautifulSoup(html)
        for bookTxt in bookSoup.findAll('div', {'id':'content'}):
            for txt in bookTxt.contents:
                bookContent =  bookContent + unicode(txt.string)
        #打印书籍内容
        bookContent = bookContent.replace("&nbsp;", "").replace("None", "")
        if IsPrint:
            print bookContent
            anlyseTime = time.time() - lastTime;
            print "读取数据花费时间:%d秒, 分析数据花费时间:%d秒"%(getDataTime, anlyseTime)
            print "==========================================================="  
    except IndexError:
        bookContent = "UNKNOWN"
        print bookContent
    return  bookContent

def DownBook(url, index):
    curPath = os.getcwd() + BOOK_CACHE_DIR_NAME
    if not os.path.exists(curPath):
        os.makedirs(curPath)

    #获取内容
    content = GetBookTxt(url, 0).encode('utf8')

    #写入文件
    fileName = str(index) + ".txt"
    fileObj = open(curPath + fileName, 'w+')
    fileObj.write(content)
    fileObj.close()

if __name__ == "__main__":
    try:
        while True:
            #获取书籍列表
            while True:
                bookTitle = raw_input("请输入要搜索的书名：")
                bookList = GetSearchBook(bookTitle)
                if bookList:
                    break
                print "没有您要找的书籍，请重新输入！"
                time.sleep(3)
                #os.system('clear')
            
            #获取书籍目录
            bookIndex = input("请输入要访问的序列：")
            #os.system('clear')
            bookUrl = bookList[bookIndex]
            bookDirList = GetBookDir(bookUrl)
            
            #获取书籍内容
            bookDirIndex = input("请输入要阅读的文章序列：")
            while True:
                #os.system('clear')
                bookDirUrl = bookUrl + bookDirList[bookDirIndex]['link']
                GetBookTxt(bookDirUrl)
                while True:
                    offect = input("[0]:上一章     [1]:下一章     [2]:缓存本书 :")
                    if offect == 0 or offect == 1 or offect == 2:
                        break
                    print "输入有误，请重新输入"
                    time.sleep(2)
                    #os.system('clear')
                    continue
                lastTime = time.time()

                #下载书籍
                if offect == 2:
                    for i in range(len(bookDirList)):
                        txtUrl = bookUrl + bookDirList[i]['link']
                        DownBook(txtUrl, i)
                print "下载完成，用时：%s"%(time.time()-lastTime)
                
                if offect == 0:
                    offect = -1
                bookDirIndex += offect
                if len(bookDirList) == bookDirIndex:
                    #os.system('clear')
                    print "恭喜你已经读完全书！"
                    while True:
                        offect = input("[0]:上一章                  [1]:退出:")
                        if offect == 0 or offect == 1:
                            break
                        print "输入有误，请重新输入"
                        time.sleep(2)
                        #os.system('clear')
                        continue
                    if offect == 1:
                        #os.system('clear')
                        break
                    bookDirIndex = bookDirIndex - 1
    except EOFError:
        print '/nWhy did you do an EOF on me?'
        sys.exit() # 退出程序
    finally:
        sys.exit() # 退出程序
