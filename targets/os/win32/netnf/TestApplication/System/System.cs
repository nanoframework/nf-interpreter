
namespace System
{
    public delegate void Action();
    public delegate void Action<T>(T value);
    public delegate T Func<T>();
    public delegate void EventHandler<T>(object sender, T e);
}
