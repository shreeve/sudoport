desc 'Build'
task :build => [:clean] do
  system "clang -c sudoport.c && clang -o sudoport sudoport.o"
  system "clang -c libsudoport.c && clang -dynamiclib -o libsudoport.dylib -install_name libsudoport.dylib libsudoport.o"
end

desc 'Clean'
task :clean do
  system "rm -f libsudoport.o libsudoport.dylib sudoport.o sudoport"
end

desc 'Install'
task :install => [:build] do
  system "cp sudoport /usr/local/bin/sudoport"
  system "cp libsudoport.dylib /usr/local/lib/"
end

desc 'Uninstall'
task :uninstall do
  system "rm -f /usr/local/bin/sudoport"
  system "rm -f /usr/local/lib/libsudoport.dylib"
end

task :default => :build

# DYLD_INSERT_LIBRARIES=/usr/local/lib/libsudoport.dylib nc -l -p 50
