for n in range(5):
    file = open(str(n)+"menu_keys.txt", 'r')
    lines = file.readlines()
    i=0
    for line in lines:
        i+=1
        keys = line[:-1].split(',')
        j=-1
        for key in keys:
            if key=="BasicPage":
                page=open("basic_page.txt", 'r')
                l=page.readlines()
                key = ''.join(l)
                page.close()
            if key=="Tableau":
                page=open("tableau.txt", 'r')
                l=page.readlines()
                key = ''.join(l)
                page.close()
            key.replace('#n#','\n')
            j+=1
            name="SD/"+str(n)+'-'+str(i)+str(j)+".KEY"
            f = open(name, "w")
            f.write(key)
            f.close()
            print(name + ' ' + key)
    file.close()
