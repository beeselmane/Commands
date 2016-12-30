package pkg;

import java.lang.Object;

public final class Class1
{
    public static final int svar_1 = 12;
    protected static String svar_2;

    private int ivar_1;
    private int ivar_2;
    private String ivar_3;
    public final Object ivar_4 = null;

    static
    {
        Class1.svar_2 = "nope";
    }

    {
        this.ivar_2 = 7;
    }

    private static int sm_i_2p(int p0, String p1)
    {
        p0 = 0;
        p1 = "test";

        int l0 = p0 + 12;

        return (l0 - 5);
    }

    public static Object sm_O_1p(double p0)
    {
        return new Double(p0);
    }

    public void im_v_0p()
    {
        this.ivar_1 = 15;
        this.ivar_3 = "string";
    }

    protected int im_i_0p()
    {
        return this.ivar_1;
    }

    public Class1(int p0)
    {
        Class1.svar_2 = new Integer(p0).toString();
    }
}
