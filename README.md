# ahdp-zl


install gsl library.

./ahdp filename-corpus filename-authors settings

filename-corpus format :
doc1-length id1:count1 id2:count2 ...
doc2-length id1:count1 id2:count2 ...
...

filename-authors format :
author_id1 author_id2
author_id2
...

settings format : 
ETA 1 
GAMMA 100
ALPHA 50

ETA - topic dirichlet parameter.
GAMMA - first level DP parameter, determin how many topics will be created.
ALPHA - second level DP parameter, determin how many topics in each document.