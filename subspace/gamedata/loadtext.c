// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "gamedata/loadtext.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cxml/cxml.h>

_objfactory_guaranteed LoadText* LoadText_Create(Subspace* ss, GameData* data)
{
    LoadText* self;
    self = objInstCreate(LoadText);

    self->ss = ss;
    self->data = objAcquire(data);

    objInstInit(self);
    return self;
}

static bool loadTextSingle(GameData *data, strref fname)
{
    cxml_root_node* root = cxml_load_file(data->ftldat, fname, true);
    if (!root)
        return false;

    cxml_set* node_set = cxml_xpath(root, "/FTL/text");

    if (!node_set) {
        cxml_destroy(root);
        return false;
    }

    cxml_for_each(nv, &node_set->items)
    {
        cxml_element_node* node = (cxml_element_node*)nv;
        cxml_attr_node* nameattr    = cxml_table_get(node->attributes, "name");
        if (!nameattr)
            continue;

        cxml_for_each(tnode, &node->children)
        {
            // this condition is pretty irrelevant since the element 'bird' has only one child
            // which is a text. However I've proceeded to add it anyway, for the sake of example.
            if (_cxml_get_node_type(tnode) == CXML_TEXT_NODE) {
                cxml_text_node* text = tnode;
                htInsert(&data->text,
                         strref,
                         (strref)cxml_string_as_raw(&nameattr->value),
                         strref,
                         (strref)cxml_string_as_raw(&text->value));
                break;
            }
        }
    }

    cxml_destroy(root);
    cxml_set_free(node_set);
    xaFree(node_set);

    return true;
}

uint32 LoadText_run(_In_ LoadText* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    bool success = true;
    string fname = 0;

    // scan all of the base (English) text files first
    foreach(vfssearch, si, self->data->ftldat, _S"/data", _S"text_*.xml", FS_File, false)
    {
        pathJoin(&fname, _S"/data", si.name);
        if (!loadTextSingle(self->data, fname)) {
            success = false;
            break;
        }
    }

    // see if there is an XML for the UI language that is selected
    string lang = 0;
    ssdStringOutD(self->ss->settings, _S"ui/lang", &lang, _S"en-us");
    strNConcat(&fname, _S"/data/text-", lang, _S".xml");
    if (vfsExist(self->data->ftldat, fname))
        loadTextSingle(self->data, fname);
    strDestroy(&lang);

    strDestroy(&fname);
    return success ? TASK_Result_Success : TASK_Result_Failure;
}

void LoadText_destroy(_In_ LoadText* self)
{
    // Autogen begins -----
    objRelease(&self->data);
    // Autogen ends -------
}

// Autogen begins -----
// clang-format off
#include "gamedata/loadtext.auto.inc"
// clang-format on
// Autogen ends -------
