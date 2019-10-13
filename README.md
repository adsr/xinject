### xinject

```
$ phpize
$ ./configure --enable-xinject
$ make
$ cat -n test.php
     1    <?php
     2
     3    function f() {
     4        echo "in f\n";
     5    }
     6
     7    function injectable() {
     8        echo "in injectable\n";
     9    }
    10
    11    f();
$ php -d zend_extension=$(pwd)/.libs/xinject.so test.php
in injectable
in f
```
