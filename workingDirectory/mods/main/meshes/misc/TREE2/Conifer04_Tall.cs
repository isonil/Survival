
singleton TSShapeConstructor(Conifer04_TallDAE)
{
   baseShape = "./Conifer04-Tall.DAE";
};

function Conifer04_TallDAE::onLoad(%this)
{
   %this.addImposter("0", "6", "0", "0", "1024", "1", "0");
}

singleton TSShapeConstructor(Conifer04_TallDAE2)
{
   baseShape = "./Conifer04_Tall.DAE";
};

function Conifer04_TallDAE2::onLoad(%this)
{
   %this.addImposter("0", "6", "0", "0", "1024", "1", "0");
}
