#!/usr/bin/python3
# -*- coding: UTF-8 -*-
#动画生成

g_ClassName = ""

class RenderItem:
    '渲染对象'
    
    def __init__(self, name, rect, type, flip, slice):
        self.Name = name
        self.Rect = rect
        self.Type = type
        self.Flip = flip
        self.Slice = slice
        self.X = None
        self.Y = None
        self.Radius = None
        self.Start = None
        self.Radian = None
    
    def Attribute(self):
        ret = ""
        if not isinstance(self.Rect, list):
            if self.Type == "rect": 
                ret += "\tRectItem m_" + self.Name + ";\n"
            elif self.Type == "arc":
                ret += "\tArcItem m_" + self.Name + ";\n"
        else:
            if self.Type == "rect": 
                ret += "\tstd::vector<RectItem> m_" + self.Name + ";\n"
            elif self.Type == "arc":
                ret += "\tstd::vector<ArcItem> m_" + self.Name + ";\n"
        return ret
    
    def ConstructLine(self):
        ret = ""
        if not isinstance(self.Rect, list):
            if self.Type == "rect":
                ret += " , m_" + self.Name + '(m_Textures.GetTexRect("' + self.Rect + '"), ' + str(self.Flip) + ', m_Billboard)\n';
            elif self.Type == "arc":
                ret += " , m_" + self.Name + '(m_Textures.GetTexRect("' + self.Rect + '"), ' + str(self.Slice) + ', m_Billboard)\n';
        return ret
    
    def Constructor(self):
        x = self.X
        y = self.Y
        if x or y:
            if not x:
                x = 0.0
            if not y:
                y = 0.0
            setPos = True
            
        setArc = self.Radius and self.Start and self.Radian

        ret = ""
        if not isinstance(self.Rect, list):
            if setPos:            
                ret += "\tm_" + self.Name + ".SetPosition(%s, %s);\n" % (x, y);
            if self.Type == "arc" and setArc:
                ret += "\tm_" + self.Name + ".SetArc(%s, %s, %s);\n" % (self.Radius, self.Start, self.Radian);
        else:
            for r in self.Rect:
                if self.Type == "rect":
                    ret += "\tm_" + self.Name + '.push_back(RectItem(m_Textures.GetTexRect("' + r + '"), ' + str(self.Flip) + ', m_Billboard));\n'
                else:
                    ret += "\tm_" + self.Name + '.push_back(ArcItem(m_Textures.GetTexRect("' + r + '"), ' + str(self.Slice) + ', m_Billboard));\n'
                    if setArc:
                        ret += "\tm_" + self.Name + ".SetArc(%s, %s, %s);\n" % (self.Radius, self.Start, self.Radian)
                if setPos:            
                    ret += "\tm_" + self.Name + "[m_" + self.Name + ".size() - 1].SetPosition(%s, %s);\n" % (x, y)
        return ret

class Equation:    
    def __init__(self, e):
        self.A = e.get("a", None)
        self.B = e.get("b", None)
        self.C = e.get("c", None)
        self.F = e.get("f", None)
        self.D = e.get("d", None)
        self.E = e.get("e", None)

    def str(self, var):
        ret = ""
        if self.A:
            ret += "%s * %s * %s" % (var, var, self.A)
        if self.B:
            if len(ret) > 0:
                ret += " + ";
            ret += "%s * %0s" % (var, self.B)
        if self.C:
            if len(ret) > 0:
                ret += " + ";
            ret += "%0s" % self.C
        if self.F:
            if len(ret) > 0:
                ret = "%s(%s)" % (self.F, ret)
                if self.D:
                    ret = "%s * %s + %s" % (self.D, ret, self.E)

        return ret

class Animation:
    '动画对象'

    def __init__(self, begin, end):
        self.Begin = begin
        self.End = end
        self.Items = []
        self.Alpha = None
        self.Scale = None
        self.X = None
        self.Y = None
        self.Radius = None
        self.Start = None
        self.Radian = None
        self.Progress = None
    
    def Implement(self):
        ret = "void " + g_ClassName + "::" + self.Name.title() + '''()
{
\tconst long ms = m_Time - m_StepStart;
}'''
        return ret

class Step:
    '一步动画'

    def __init__(self, name):
        self.Name = name
        self.Animations = []
    
    def Define(self):
        return "\tbool " + self.Name.title() + "();\n";

    def Call(self):
        return "\t\t\t\t" + self.Name.title() + "();\n";     

    def Implement(self, objs):
        end = 0
        for a in self.Animations:
            if end < a.End:
                end = a.End

        ret = "bool " + g_ClassName + "::" + self.Name.title() + "()\n"
        ret += "{\n"
        
        if end > 0:
            ret += "\tconst long ms = StepTime(" + str(end) + ");\n"

        alphaChanged = True;
        for a in self.Animations:
            if a.Begin != a.End:
                ret += "\tif(ms >= " + str(a.Begin) + " && ms < " + str(a.End) + ")\n"
            else:
                ret += "if(true)\n"
            
            ret += "\t{\n"

            if a.Alpha:
                ret += "\t\tfloat alpha = " + a.Alpha.str("ms") + ";\n"
                ret += "\t\tm_Billboard->SetAlpha(alpha);\n"
                alphaChanged = True
            elif alphaChanged:
                ret += "\t\tm_Billboard->SetAlpha(1.0f);\n"
                alphaChanged = False
            
            if a.Scale:
                ret += "\t\tfloat scale = " + a.Scale.str("ms") + ";\n"
                for i in a.Items:
                    ret += "\t\tm_" + i + ".SetScale(scale);\n"
            
            if a.X and a.Y:
                ret += "\t\tfloat x = " + a.X.str("ms") + ";\n"
                ret += "\t\tfloat y = " + a.Y.str("ms") + ";\n"
                for i in a.Items:
                    ret += "\t\tm_" + i + ".SetPosition(x, y);\n"
            elif a.X:
                ret += "\t\tfloat x = " + a.X.str("ms") + ";\n"
                for i in a.Items:
                    ret += "\t\tm_" + i + ".SetPosition(x, m_" + i + ".GetY());\n"
            elif a.Y:
                ret += "\t\tfloat y = " + a.Y.str("ms") + ";\n"
                for i in a.Items:
                    ret += "\t\tm_" + i + ".SetPosition(m_" + i + ".GetX(), y);\n"
                    
            if a.Radius and a.Start and a.Radian:
                ret += "\t\tfloat radius = " + a.Radius.str("ms") + ";\n"
                ret += "\t\tfloat start = " + a.Start.str("ms") + ";\n"
                ret += "\t\tfloat radian = " + a.Radian.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(radius, start, radian);\n"
            elif a.Radius and a.Start:
                ret += "\t\tfloat radius = " + a.Radius.str("ms") + ";\n"
                ret += "\t\tfloat start = " + a.Start.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(radius, start, m_" + i + ".GetRadian());\n"
            elif a.Radius and a.Radian:
                ret += "\t\tfloat radius = " + a.Radius.str("ms") + ";\n"
                ret += "\t\tfloat radian = " + a.Radian.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(radius, m_" + i + ".GetBegin(), radian);\n"
            elif a.Start and a.Radian:
                ret += "\t\tfloat start = " + a.Start.str("ms") + ";\n"
                ret += "\t\tfloat radian = " + a.Radian.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(m_" + i + ".GetRadius(), start, radian);\n"
            elif a.Radius:
                ret += "\t\tfloat radius = " + a.Radius.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(radius, m_" + i + ".GetBegin(), m_" + i + ".GetRadian());\n"
            elif a.Start:
                ret += "\t\tfloat start = " + a.Start.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(m_" + i + ".GetRadius(), start, m_" + i + ".GetRadian());\n"
            elif a.Radian:
                ret += "\t\tfloat radian = " + a.Radian.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetArc(m_" + i + ".GetRadius(), m_" + i + ".GetBegin(), radian);\n"
            
            if a.Progress:
                ret += "\t\tfloat p = " + a.Progress.str("ms") + ";\n"
                for i in a.Items:
                    if objs[i].Type == "arc":
                        ret += "\t\tm_" + i + ".SetProgress(p);\n"

            for i in a.Items:
                ret += "\t\tm_" + i + ".Draw();\n"
            ret += "\t}\n"
        
        if end == 0:
            ret += "\tDisplayInstrument();\n"
            ret += "\treturn true;\n"
        else:
            ret += "\tif(ms < 0)\n"
            ret += "\t{\n"
            ret += "\t\tPreviousStep();\n"
            ret += "\t\treturn false;"
            ret += "\t}\n"
            ret += "\tif(ms < " + str(end) + ")\n"
            ret += "\t{\n"
            ret += "\t\tDisplayInstrument();\n"
            ret += "\t\treturn true;\n"
            ret += "\t}\n"
            ret += "\telse"
            ret += "\t{\n"
            ret += "\t\tNextStep();\n"
            ret += "\t\treturn false;"
            ret += "\t}\n"
        
        ret += "}\n"
        
        return ret

import json

def SetAttr(dst, att0, src, att1):
    a = src.get(att1, None)
    if a:
        setattr(dst, att0, a)

def SetEquation(dst, att0, src, att1):
    a = src.get(att1, None)
    if a:
        setattr(dst, att0, Equation(a))

def LoadObjects(filename):
    file = open(filename, 'r')
    
    from collections import OrderedDict
    data = json.load(file, object_pairs_hook=OrderedDict)
    
    itemList = []
    stepList = []

    items = data.get('items', None)
    if items:    
        for v in items:
            flip = 0;
            if v.get("VerticalFlip", False):
                flip = 1
            if v.get("HorizonalFlip", False):
                flip += 2;
            item = RenderItem(v["name"], v['rect'], v["type"], flip, v.get("slice", 1))
            SetAttr(item, "X", v, "x")
            SetAttr(item, "Y", v, "y")
            SetAttr(item, "Radius", v, "radius")
            SetAttr(item, "Start", v, "start")
            SetAttr(item, "Radian", v, "radian")
            itemList.append(item)
    
    steps = data.get('steps', None)
    if steps:
        for k, v in steps.items():
            step = Step(k)
            for a in v:
                ani = Animation(a["begin"], a["end"])
                items = a.get("items", None)
                if items:
                    ani.Items = items
                
                SetEquation(ani, "X", a, "x")
                SetEquation(ani, "Y", a, "y")
                SetEquation(ani, "Alpha", a, "alpha")
                SetEquation(ani, "Scale", a, "scale")

                SetEquation(ani, "Radius", a, "radius")
                SetEquation(ani, "Start", a, "start")
                SetEquation(ani, "Radian", a, "radian")
                SetEquation(ani, "Progress", a, "progress")
                
                step.Animations.append(ani)
            stepList.append(step)
            
    return (itemList, stepList)    

import sys
if len(sys.argv) != 5:
    print("Usage:" + sys.argv[0] + " json header implement cppclass")
    sys.exit(1)

g_ClassName = sys.argv[4];
objectList, animateList = LoadObjects(sys.argv[1])

objectMap = {}

for o in objectList:
    objectMap[o.Name] = o

#write header file
tf = open(sys.argv[2], "rt")
template = tf.read()

template = template.replace("%headername%", g_ClassName.upper())
template = template.replace("%classname%", g_ClassName)

out = ""
for a in animateList:
    out += a.Define()
template = template.replace("%defines%", out)

out = ""
for o in objectList:
    out += o.Attribute()
template = template.replace("%items%", out)

out = open(g_ClassName.lower() + ".h", "wt")
out.write(template)

#write implement file
tf = open(sys.argv[3], "rt")
template = tf.read()

template = template.replace("%classname%", g_ClassName)

out = ""
for o in objectList:
    out += o.ConstructLine()
template = template.replace("%constructline%", out)

out = ""
for o in objectList:
    out += o.Constructor()
template = template.replace("%constructor%", out)

out = ""
index = 0
for a in animateList:
	index += 1
	out += "\t\t\tcase " + str(index) + ":\nrendered = " + a.Call() + "\t\t\t\tbreak;\n"
template = template.replace("%steps%", out)

out = ""
for a in animateList:
    out += a.Implement(objectMap)
template = template.replace("%implement%", out)

out = open(g_ClassName.lower() + ".cpp", "wt")
out.write(template)
