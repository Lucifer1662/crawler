
import os
import sys
import collections
from sys import platform

def checkIfDuplicates(listOfElems):
    if len(listOfElems) == len(set(listOfElems)):
        return False
    else:
        return True

answers_file = open(sys.argv[1], "r")

answers = answers_file.read().splitlines()

main_url = answers[0]
answers = sorted(answers)


if platform == "linux" or platform == "linux2":
    # linux
    stream = os.popen('./crawler ' + main_url)
elif platform == "darwin":
    # OS X
    stream = os.popen('./crawler ' + main_url)
elif platform == "win32":
    # Windows...
    stream = os.popen('crawler ' + main_url)


results = stream.read().splitlines()
results = sorted(results)

matched = results == answers
wereUnique = not checkIfDuplicates(results)

if(matched and wereUnique):
    print("Passed")
else:
    print("Failed")
    if(not matched):
        print("Exprected:")
        for answer in answers:
            print(answer)
        print("Actual:")
        for result in results:
            print(result)
    if(not wereUnique):
        print("There was duplicates")
        print([item for item, count in collections.Counter(results).items() if count > 1])