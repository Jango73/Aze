
rm -rf GitMergeTestFolder
mkdir GitMergeTestFolder

rm -rf AzeMergeTestFolder
mkdir AzeMergeTestFolder

#------------------------------------------------------------------------------

cd GitMergeTestFolder
git init

echo $'CA1\n--' > MergeTest.txt
git add MergeTest.txt
git commit -m "CA1"

git branch A
git checkout A

echo $'CA1\nCA2\n--' > MergeTest.txt
git add MergeTest.txt
git commit -m "CA2"

git branch B

echo $'CA1\nCA2\nCA3\n--' > MergeTest.txt
git add MergeTest.txt
git commit -m "CA3"

git --no-pager log --pretty=oneline

git checkout B

echo $'CA1\nCA2\n--\nCB1' > MergeTest.txt
git add MergeTest.txt
git commit -m "CB1"

echo $'CA1\nCA2\n--\nCB1\nCB2' > MergeTest.txt
git add MergeTest.txt
git commit -m "CB2"

echo $'CA1\nCA2\n--\nCB1\nCB2\nCB3' > MergeTest.txt
git add MergeTest.txt
git commit -m "CB3"

git --no-pager log --pretty=oneline

git branch C
git checkout C

echo $'CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1' > MergeTest.txt
git add MergeTest.txt
git commit -m "CC1"

echo $'CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1\nCC2' > MergeTest.txt
git add MergeTest.txt
git commit -m "CC2"

git checkout A

git merge B --commit --no-edit
git --no-pager log --pretty=oneline

git merge C --commit --no-edit
git --no-pager log --pretty=oneline

#------------------------------------------------------------------------------

cd ../AzeMergeTestFolder
aze init

echo $'CA1\n--' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CA1"

aze create-branch A
aze set-branch A

echo $'CA1\nCA2\n--' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CA2"

aze create-branch B

echo $'CA1\nCA2\nCA3\n--' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CA3"

# aze log

aze set-branch B

echo $'CA1\nCA2\n--\nCB1' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CB1"

echo $'CA1\nCA2\n--\nCB1\nCB2' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CB2"

echo $'CA1\nCA2\n--\nCB1\nCB2\nCB3' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CB3"

# aze log

aze create-branch C
aze set-branch C

echo $'CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CC1"

echo $'CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1\nCC2' > MergeTest.txt
aze stage MergeTest.txt
aze commit --message "CC2"

aze set-branch A

aze merge B
# aze log

aze merge C
# aze log

#------------------------------------------------------------------------------

cd ..
# diff ./GitMergeTestFolder/MergeTest.txt ./AzeMergeTestFolder/MergeTest.txt
cat ./GitMergeTestFolder/MergeTest.txt
echo "----------------"
cat ./AzeMergeTestFolder/MergeTest.txt
