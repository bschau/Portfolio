[TestClass]
public class NumbersTests
{
    // Add the MsTest.TestAdapter and MsTest.TestFramework nuget packages to your unittest solution.
    // Then you can annotate your method with DataTestMethod and use DataRow's to pass data to the method:
    [DataRow(0, 0)]
    [DataRow(1, 1)]
    [DataRow(2, 1)]     // Yep, this wil fail :-)
    [DataTestMethod]
    public void VerifyThatTwoSelectedNumbersAreEqual(int number1, int number2)
    {
        Assert.AreEqual(number1, number2);
    }
}