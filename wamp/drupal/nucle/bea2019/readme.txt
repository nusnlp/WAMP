This directory contains the official NUCLE training file used in the BEA2019 shared task.

Specifically, nucle.train.gold.bea19.m2 is the same as the NUCLE M2 file used in the CoNLL2014 shared task, except the error types have been automatically standardised using the ERRANT framework: https://github.com/chrisjbryant/errant

The official BEA19 file was generated using the following command in Python 3.5:

python3 errant/m2_to_m2.py -gold <nucle.conll2014.m2> -out nucle.train.gold.bea19.m2

This used spacy v1.9.0 and the en_core_web_sm-1.2.0 model.