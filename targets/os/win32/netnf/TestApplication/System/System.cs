
namespace System
{
    public delegate void Action();
    public delegate void Action<T>(T value);
    public delegate T Func<T>();
    public delegate T2 Func<T1, T2>(T1 t1);
    public delegate void EventHandler<T>(object sender, T e);
}
