sudoport
========

## Summary

The sudoport utility allows non-root users, who are able to run the
sudo command, to bind privileged ports on Mac OSX. It accomplishes this
by injecting a dynamic library prior to executing the supplied command.
This dynamic library intercepts all calls to bind() on privileged ports
and, instead, forks a child process that runs sudo and the sudoport
helper, which does the actual binding of the privileged port as root.
Any user that can already run sudo can run sudoport, but it's suggested
that you disable the password prompt for such users. If the admin group
is how you are controlling access to sudo, then modify it as follows:

  ```%admin  ALL=(ALL) NOPASSWD: ALL```

This allows commands like the following to bind() on privileged ports:

  ```sudoport nc -l -p 443```

## Installing

Compilation requires ruby and rake to be installed. Just type:

  ```rake install```

This will build and install sudoport and libsudoport.dylib to /usr/local.
You can also run the following rake tasks:

```
rake           # builds in the local directory
rake clean     # cleans current directory
rake uninstall # removes from /usr/local
```

## Credits

* [authbind](http://www.chiark.greenend.org.uk/ucgi/~ian/git/authbind.git/)
* [Castaglia/MacOSX-authbind](https://github.com/Castaglia/MacOSX-authbind)
* Steve Shreeve (steve.shreeve@gmail.com)
