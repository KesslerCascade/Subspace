// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "optionsfeatures.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/subspaceui.h"

_objfactory_guaranteed FeaturesPage* FeaturesPage_create(SubspaceUI* ui)
{
    FeaturesPage* self;
    self = objInstCreate(FeaturesPage);

    self->ui = ui;
    self->ss = ui->ss;
    self->visible = true;

    self->name = _S"features";
    strDup(&self->title, langGet(self->ss, _S"options_features"));
    strDup(&self->imgname, _S"IMAGE_PUZZLE_PIECE_SMALL");

    objInstInit(self);
    return self;
}

extern bool OptionsPage_make(_In_ OptionsPage* self, Ihandle* list);   // parent
#define parent_make(list) OptionsPage_make((OptionsPage*)(self), list)
bool FeaturesPage_make(_In_ FeaturesPage* self, Ihandle* list)
{
    self->h = IupVbox(IupHbox(IupFill(), NULL), IupFill(), NULL);

    iupLoadImage(self->ss,
        _S"IMAGE_PUZZLE_PIECE_SMALL",
        _S"svg",
        _S"subspace:/puzzle-piece-small.svg",
        list);

    return true;
}

// Autogen begins -----
#include "optionsfeatures.auto.inc"
// Autogen ends -------
