#!/bin/bash

php -d zend_extension=$(pwd)/.libs/xinject.so test.php
