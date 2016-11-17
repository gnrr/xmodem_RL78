# ifn  = 'lisp1.7z'
ofn = 'c:/Users/hogege/Documents/content.c'

sz = File.size(ifn)
lines = ["#include \"r_cg_userdefine.h\"\n",
         "\n",
         "const u4 content_sz = %d;\t\t\t// %d bytes\n" % [sz, sz],
         "const u1 content[] = {\n",
         " " * 4]

# read binary file
f = File.open(ifn, 'rb')
cnt = 0
f.each_byte do |c|
    lines += ["0x%02X, " % c]
    if cnt % 10 == 9 then
        lines += [" " * 4 + "// %8d\n" % (cnt + 1) + " " * 4]
    elsif cnt == sz - 1 then
        lines += [" " * 34 + "// %8d\n" % (cnt + 1)]
    end
    cnt += 1
end
lines += ["};\n"]

f.close

# write c-source file
f = File.open(ofn, 'w')
i = 0
lines.each do |l|
    f.print l
    
    i += 1
    if i >= 1024 then
        print '.'
        i = 0
    end
end
f.close

puts "\n--> %s" % ofn
