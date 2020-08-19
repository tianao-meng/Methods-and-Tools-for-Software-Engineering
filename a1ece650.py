


import sys
# YOUR CODE GOES HERE

idD = {}
edgeD = {}

def cerr(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)

def intersect(l1, l2):
    x1, y1 = l1[0][0], l1[0][1]
    x2, y2 = l1[1][0], l1[1][1]
    x3, y3 = l2[0][0], l2[0][1]
    x4, y4 = l2[1][0], l2[1][1]

    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    if xden == 0:
        return []
    xcoor =  xnum / xden

    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
    if yden == 0:
        return []
    ycoor = ynum / yden


    xcoor = round(xcoor, 2)
    ycoor = round(ycoor, 2)

    compare = (((xcoor - x1) * (xcoor - x2) <= 0) and
               ((ycoor - y1) * (ycoor - y2) <= 0) and
               ((xcoor - x3) * (xcoor - x4) <= 0) and
               ((ycoor - y3) * (ycoor - y4) <= 0))

    if compare:
        return [xcoor, ycoor]
    return []




def getPoint(info):
    lst = info.split(",")
    if len(lst) != 2:
        cerr("point info wrong")
    return [round(float(lst[0]), 2), round(float(lst[1]), 2)]

def lineToLst(line):
    idx1 = line.find("\"")
    if (idx1 == -1 or idx1 == 0 or line[idx1-1] != " "):
        cerr("lineToLst error")
    idx2 = line.find("\"", idx1 + 1)
    if (idx2 == -1 or (idx2 +1 < len(line) and line[idx2+1] != " ")):
        cerr("lineToLst error")


    sname = line[idx1+1:idx2]
    sname = sname.lower()
    rest = line[idx2+1:]
    lst = []
    while True:
        idx1 = rest.find("(")
        if idx1 == -1:
            break

        idx2 = rest.find(")")
        if idx2 == -1:
            cerr("point () wrong")
        pointInfo = rest[idx1+1:idx2]
        lst.append(getPoint(pointInfo))
        rest = rest[idx2+1:]
    return sname, lst

sd = {}

def getEdge(key):
    i = 0
    lst = sd[key]
    ans = []
    while i <= len(lst)-2:
        ans.append([lst[i], lst[i+1]])
        i+=1
    return ans

def createNewDict():
    nd = {}
    keys = list(sd.keys())
    for k in keys:
        edgeLst = getEdge(k)
        nd[k] = edgeLst
    return nd

def isNode(edge, node):
    n1 = edge[0]
    n2 = edge[1]
    if node[0] == n1[0] and node[1] == n1[1]:
        return True
    if node[0] == n2[0] and node[1] == n2[1]:
        return True
    return False

def rmD(lst):
    nlst = []
    for i in lst:
        if i not in nlst:
            nlst.append(i)
    return nlst

def run():
    nd = createNewDict()
    coorLst = []
    keys = list(nd.keys())
    fi = 0
    while fi < len(keys):
        fedgeLst = nd[keys[fi]]
        fj = 0
        while fj < len(fedgeLst):
            fedge = fedgeLst[fj]

            ti = fi + 1
            while ti < len(keys):
                tedgeLst = nd[keys[ti]]
                tj = 0
                while tj < len(tedgeLst):
                    tedge = tedgeLst[tj]

                    coor = intersect(fedge, tedge)
                    if coor != []:
                        coorLst.append(coor)
                        if not isNode(fedge, coor):
                            temp = fedgeLst[fj][1]
                            fedgeLst[fj][1] = coor[:]
                            newfEdge = [coor[:], temp]
                            fedgeLst = fedgeLst[:fj+1] + [newfEdge] + fedgeLst[fj+1:]
                            nd[keys[fi]] = fedgeLst[:]
                            fedge = fedgeLst[fj]
                        if not isNode(tedge, coor):
                            temp = tedgeLst[tj][1]
                            tedgeLst[tj][1] = coor[:]
                            newtEdge = [coor[:], temp]
                            tedgeLst = tedgeLst[:tj+1] + [newtEdge] + tedgeLst[tj+1:]
                            nd[keys[ti]] = tedgeLst[:]
                            tedge = tedgeLst[tj]

                            ti = fi+1
                            tedgeLst = nd[keys[ti]]
                            tedge = tedgeLst[0]
                            tj = 0
                            continue
                    tj+=1
                ti += 1
            fj += 1
        fi += 1

    coorLst = rmD(coorLst)
    edgeLst = []
    localEdge = []
    for street in keys:
        for edge in nd[street]:
            if (edge[0] in coorLst or edge[1] in coorLst) and edge not in edgeLst:
                edgeLst.append(edge)
            if (edge[0] in coorLst or edge[1] in coorLst) and edge not in localEdge:
                localEdge.append(edge)

    for edge in edgeLst:
        for node in edge:
            if str(node) not in idD:
                n = len(idD) + 1
                idD[str(node)] = n
                edgeD[n] = node

    sopV(localEdge)
    print("E = {")
    context = ""
    for edge in edgeLst:
        context += ("  <" + str(idD[str(edge[0])]) + "," + str(idD[str(edge[1])]) +">") + ",\n"
    if context != "":
        print(context[:-2])
    print("}")


def sopV(localEdge) :
    nodeLst = []
    for e in localEdge:
        if e[0] not in nodeLst:
            nodeLst.append(e[0])
        if e[1] not in nodeLst:
            nodeLst.append(e[1])
    idLst = list(map(lambda x: idD[str(x)] , nodeLst))
    idLst.sort()

    print("V = {")
    for i in idLst:
        print("  "+str(i)+":  (%.2f,%.2f)" % (edgeD[i][0],edgeD[i][1]))
    print("}")

def core(line):
    lst = line.split()
    code = lst[0]
    if code == "a":
        sname, nlst = lineToLst(line)
        if len(nlst) < 2:
            cerr("a code with no node")
        if sname in sd:
            print(sname)
            cerr("a code wrong: already exist this street")
        sd[sname] = nlst
        return

    if code == "r":
        sname, nlst = lineToLst(line)
        if nlst != []:
            cerr("r code type wrong")
        if sname not in sd:
            cerr("r code wrong: cannot find this street")

        del sd[sname]
        return

    if code == "c":
        sname, nlst = lineToLst(line)
        if len(nlst) < 2:
            cerr("c code type wrong, with no node")
        if sname not in sd:
            cerr("c code wrong: cannot find this street")

        sd[sname] = nlst
        return

    if code == "g":
        run()
        return

    cerr("ERROR: cannot find the code " + line)

def main():
    while True:
        userIn = input()
        if userIn == "":
            break
        core(userIn)

if __name__ == '__main__':
    main()
