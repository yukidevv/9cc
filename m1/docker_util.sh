#!/usr/bin/env bash

#中に入りたい
if [[ $1 = "in" ]]; then
  docker run --rm -it -v $HOME/fun/9cc:/9cc compilebook
else
  docker run --rm -v $HOME/fun/9cc:/9cc -w /9cc compilebook make test
fi

