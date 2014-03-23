import net.tdlsoftware.libabf.AbfDecoder;
class testabf {
static {
System.load("/home/peter/abf/swig/java/libabf.so");
}

public static void main(String[] args) {
//System.out.println(System.Library.path);
System.out.println("Hello");
System.out.println(args[0]);
AbfDecoder AD = new AbfDecoder(args[0]);
System.out.println("Right, let's try and print out a bit of information about our audio book.");
System.out.println("Title:");
System.out.println(AD.GetTitle());

}
}

