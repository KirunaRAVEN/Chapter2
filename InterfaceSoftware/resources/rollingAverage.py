'''
    AUTHOR: Louis-Hendrik Barboutie
    CONTACT: louis.barboutie@gmail.com, loubar-3@student.ltu.se

    SUMMARY: This function computes the moving average over the points fed to it in the dataList, intended to be the data displayed on screen
'''    
def rollingAverage(dataList):
    rollingAverage = 0
    for i in range(len(dataList)):
        rollingAverage += dataList[i]
    rollingAverage /= len(dataList)

    return rollingAverage