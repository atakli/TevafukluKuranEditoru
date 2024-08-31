with open('quran-simple-clean.txt', 'r', encoding='utf-8') as file:
	lines = file.readlines()
	
pages = []
besmele = lines[0].strip()
for index, i in enumerate(lines):
    if besmele in i:
        pages.append(index)
        # print(index)
        # print(i)
pages.remove(3188)  # because an-naml has besmele inside the sura
print(len(pages))

with open('quran-simple-clean-besmelesiz.txt', 'r', encoding='utf-8') as file:
	lines = file.readlines()
    
for i in range(1, 114):
    sure = open(f"suras/{i}.txt", 'w', encoding='utf-8')
    sure.writelines(lines[pages[i - 1] : pages[i]])
    sure.close()
    
sure = open(f"suras/114.txt", 'w', encoding='utf-8')    # for the last sura
sure.writelines(lines[pages[113] : ])
sure.close()