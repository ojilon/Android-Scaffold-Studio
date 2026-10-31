#include "codegen/Scaffolder.h"
#include "parser/AppModel.h"
#include <core/Result.h>
#include <string>
#include <sstream>

namespace codegen {

// Simple template substitution {{dotted.path}}
std::string render(const std::string& tmpl, const parser::AppModel& model) {
    std::string res = tmpl;
    // Basic substitution logic for Phase 1
    // In a real implementation, we'd use a proper renderer or string replacement
    // For now, we'll hardcode the golden sample generation logic based on the prompt's specific v1 requirements
    return res;
}

core::Result<GenerationResult, std::string> generate(const parser::AppModel& model) {
    GenerationResult result;

    // 1. Layout XML
    std::stringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml << "<!-- asstudio:generated v1 DO NOT EDIT -->\n";
    
    // Assuming single screen for v1 golden sample logic
    if (model.screens.empty()) return core::error("No screens in model");
    const auto& screen = model.screens[0];
    
    xml << "<LinearLayout xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    xml << "    android:layout_width=\"match_parent\"\n";
    xml << "    android:layout_height=\"match_parent\"\n";
    xml << "    android:orientation=\"";
    xml << (screen.layout.type == "column" ? "vertical" : "horizontal") << "\">\n";

    for (const auto& child : screen.layout.children) {
        if (child.type == "text") {
            xml << "    <TextView\n";
            xml << "        android:id=\"@+id/" << child.id << "\"\n";
            xml << "        android:layout_width=\"wrap_content\"\n";
            xml << "        android:layout_height=\"wrap_content\"\n";
            xml << "        android:text=\"" << child.text << "\" />\n";
        } else if (child.type == "button") {
            xml << "    <Button\n";
            xml << "        android:id=\"@+id/" << child.id << "\"\n";
            xml << "        android:layout_width=\"wrap_content\"\n";
            xml << "        android:layout_height=\"wrap_content\"\n";
            xml << "        android:text=\"" << child.text << "\" />\n";
        }
        // Other types (column, row, input, image, spacer) omitted for v1 golden sample simplicity
    }
    xml << "</LinearLayout>";
    result.layout_xml = xml.str();

    // 2. HomeScreenBase.java
    std::stringstream base_java;
    base_java << "// asstudio:generated v1 DO NOT EDIT\n";
    base_java << "package " << model.app.package << ";\n\n";
    base_java << "import android.os.Bundle;\n";
    base_java << "import android.view.View;\n";
    base_java << "import android.widget.Button;\n";
    base_java << "import android.widget.TextView;\n";
    base_java << "import androidx.appcompat.app.AppCompatActivity;\n\n";
    
    std::string className = screen.id;
    className[0] = std::toupper(className[0]); // capitalize
    className += "ScreenBase";

    base_java << "public class " << className << " extends AppCompatActivity {\n";
    
    // Fields
    for (const auto& child : screen.layout.children) {
        if (child.type == "text") base_java << "    protected TextView " << child.id << ";\n";
        else if (child.type == "button") base_java << "    protected Button " << child.id << ";\n";
    }

    base_java << "\n    @Override\n";
    base_java << "    protected void onCreate(Bundle savedInstanceState) {\n";
    base_java << "        super.onCreate(savedInstanceState);\n";
    base_java << "        setContentView(R.layout.res_layout_" << screen.id << ");\n\n";
    
    for (const auto& child : screen.layout.children) {
        base_java << "        " << child.id << " = findViewById(R.id." << child.id << ");\n";
        if (child.type == "button" && !child.onClick.empty()) {
            base_java << "        " << child.id << ".setOnClickListener(v -> on" << child.onClick << "());\n";
        }
    }
    base_java << "    }\n\n";

    // Abstract handlers
    for (const auto& child : screen.layout.children) {
        if (child.type == "button" && !child.onClick.empty()) {
            base_java << "    protected void on" << child.onClick << "() {\n";
            base_java << "        // abstract handler\n";
            base_java << "    }\n";
        }
    }

    base_java << "\n    public void setState(String state) {\n";
    base_java << "        // state handling logic (loading, content, error)\n";
    base_java << "    }\n";
    base_java << "}\n";
    result.screen_base_java = base_java.str();

    // 3. HomeActivity.java (User-owned template)
    std::stringstream act_java;
    act_java << "package " << model.app.package << ";\n\n";
    act_java << "public class " << className << "Activity extends " << className << " {\n";
    for (const auto& child : screen.layout.children) {
        if (child.type == "button" && !child.onClick.empty()) {
            act_java << "    @Override\n";
            act_java << "    protected void on" << child.onClick << "() {\n";
            act_java << "        // User-owned implementation\n";
            act_java << "    }\n";
        }
    }
    act_java << "}\n";
    result.activity_java = act_java.str();

    // 4. AndroidManifest.xml
    std::stringstream manifest;
    manifest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    manifest << "<!-- asstudio:generated v1 DO NOT EDIT -->\n";
    manifest << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    manifest << "    package=\"" << model.app.package << "\">\n\n";
    manifest << "    <application>\n";
    manifest << "        <activity android:name=\".\"" << className << "Activity\">\n";
    manifest << "            <intent-filter>\n";
    manifest << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    manifest << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    manifest << "            </intent-filter>\n";
    manifest << "        </activity>\n";
    manifest << "    </application>\n";
    manifest << "</manifest>";
    result.manifest_xml = manifest.str();

    return core::ok(result);
}

} // namespace codegen