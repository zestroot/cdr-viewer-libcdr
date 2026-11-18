/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libcdr project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __CDRCONTENTCOLLECTOR_H__
#define __CDRCONTENTCOLLECTOR_H__

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <stack>
#include <queue>
#include <string>

#include <librevenge/librevenge.h>

#include "CDROutputElementList.h"
#include "CDRTransforms.h"
#include "CDRTypes.h"
#include "CDRPath.h"
#include "CDRCollector.h"

namespace libcdr
{

class CDRContentCollector : public CDRCollector
{
public:
  CDRContentCollector(CDRParserState &ps, librevenge::RVNGDrawingInterface *painter, bool reverseOrder = true);
  ~CDRContentCollector() override;

  // collector functions
  void collectPage(unsigned level) override;
  void collectObject(unsigned level) override;
  void collectGroup(unsigned level) override;
  void collectVect(unsigned level) override;
  void collectOtherList() override;
  void collectPath(const CDRPath &path) override;
  void collectLevel(unsigned level) override;
  void collectTransform(const CDRTransforms &transforms, bool considerGroupTransform) override;
  void collectFillStyle(unsigned,const CDRFillStyle &) override {}
  void collectFillStyleId(unsigned id) override;
  void collectLineStyle(unsigned,const CDRLineStyle &) override {}
  void collectLineStyleId(unsigned id) override;
  void collectRotate(double angle, double cx, double cy) override;
  void collectFlags(unsigned flags, bool considerFlags) override;
  void collectPageSize(double, double, double, double) override {}
  void collectPolygonTransform(unsigned numAngles, unsigned nextPoint, double rx, double ry, double cx, double cy) override;
  void collectBitmap(unsigned imageId, double x1, double x2, double y1, double y2) override;
  void collectBmp(unsigned, unsigned, unsigned, unsigned, unsigned, const std::vector<unsigned> &, const std::vector<unsigned char> &) override {}
  void collectBmp(unsigned, const std::vector<unsigned char> &) override {}
  void collectBmpf(unsigned, unsigned, unsigned, const std::vector<unsigned char> &) override {}
  void collectPpdt(const std::vector<std::pair<double, double> > &points, const std::vector<unsigned> &knotVector) override;
  void collectFillTransform(const CDRTransforms &fillTrafo) override;
  void collectFillOpacity(double opacity) override;
  void collectPolygon() override;
  void collectSpline() override;
  void collectColorProfile(const std::vector<unsigned char> &) override {}
  void collectBBox(double x0, double y0, double x1, double y1) override;
  void collectSpnd(unsigned spnd) override;
  void collectVectorPattern(unsigned id, const librevenge::RVNGBinaryData &data) override;
  void collectPaletteEntry(unsigned, unsigned, const CDRColor &) override {}
  void collectText(unsigned, unsigned, const std::vector<unsigned char> &,
                   const std::vector<unsigned char> &, const std::map<unsigned, CDRStyle> &) override {}
  void collectArtisticText(double x, double y, bool objectStyle = false) override;
  void collectParagraphText(double x, double y, double width, double height) override;
  void collectStld(unsigned, const CDRStyle &) override {}
  void collectStyleId(unsigned styleId) override;
  void collectLayer(const std::string &id) override;
  void collectLayerEnd(unsigned level) override;
  void collectClipList(unsigned level) override;
  void collectPowerClip(unsigned id) override;

private:
  CDRContentCollector(const CDRContentCollector &);
  CDRContentCollector &operator=(const CDRContentCollector &);

  // helper functions
  void _startDocument();
  void _endDocument();
  void _startPage(double width, double height);
  void _endPage();
  void _flushCurrentPath();
  double _textRotation();
  void _textScale(double &sx, double &sy);

  void _fillProperties(librevenge::RVNGPropertyList &propList);
  librevenge::RVNGBinaryData _monoImage(const librevenge::RVNGBinaryData &bmp);
  void _lineProperties(librevenge::RVNGPropertyList &propList);

  // A line marker as written out, and what it was made from
  struct WrittenMarker
  {
    std::shared_ptr<const CDRPath> marker;
    CDRTransforms transforms;
    bool hasGroupTransforms;
    CDRTransforms groupTransforms;
    librevenge::RVNGString path;
    librevenge::RVNGString viewBox;
    WrittenMarker()
      : marker(), transforms(), hasGroupTransforms(false), groupTransforms(), path(), viewBox() {}
  };
  void _writeMarker(WrittenMarker &written, const std::shared_ptr<const CDRPath> &marker, const CDRTransform &mirror);
  const librevenge::RVNGString &_markerPath(const std::shared_ptr<const CDRPath> &marker);
  void _generateBitmapFromPattern(librevenge::RVNGBinaryData &bitmap, const CDRPattern &pattern, const CDRColor &fgColor, const CDRColor &bgColor);

  librevenge::RVNGDrawingInterface *m_painter;

  bool m_isDocumentStarted;
  bool m_isPageProperties;
  bool m_isPageStarted;
  bool m_ignorePage;

  CDRPage m_page;
  unsigned m_pageIndex;
  CDRFillStyle m_currentFillStyle;
  CDRLineStyle m_currentLineStyle;
  unsigned m_spnd;
  unsigned m_currentObjectLevel, m_currentGroupLevel, m_currentVectLevel, m_currentPageLevel, m_currentStyleId;
  CDRImage m_currentImage;
  bool m_currentImageMono;   // a 1-bit bitmap (see _monoImage)
  const std::vector<CDRTextLine> *m_currentText;
  bool m_currentTextObjectStyle;   // its characters take the object's fill and outline (see collectArtisticText)
  CDRBox m_currentBBox;
  CDRBox m_currentTextBox;

  CDRPath m_currentPath;
  CDRTransforms m_currentTransforms;
  CDRTransforms m_fillTransforms;
  std::unique_ptr<CDRPolygon> m_polygon;
  bool m_isInPolygon;
  bool m_isInSpline;
  std::stack<CDROutputElementList> *m_outputElementsStack;
  std::stack<CDROutputElementList> m_contentOutputElementsStack;
  std::stack<CDROutputElementList> m_fillOutputElementsStack;
  std::queue<CDROutputElementList> *m_outputElementsQueue;
  std::queue<CDROutputElementList> m_contentOutputElementsQueue;
  std::queue<CDROutputElementList> m_fillOutputElementsQueue;
  std::stack<unsigned> m_groupLevels;
  std::stack<CDRTransforms> m_groupTransforms;
  WrittenMarker m_writtenStartMarker;
  WrittenMarker m_writtenEndMarker;
  // each arrowhead's outline as SVG path data in its own units
  std::map<std::shared_ptr<const CDRPath>, librevenge::RVNGString> m_markerPaths;
  CDRSplineData m_splineData;
  double m_fillOpacity;
  bool m_reverseOrder;

  // master layers (CorelDRAW X6+): their content in drawing order, by layer id, and the ids in
  // the master page's order. Each page lists the master layers it shows among its own layers,
  // which places them in its stacking order (a background under the page's objects).
  std::map<std::string, std::vector<CDROutputElementList>> m_masterLayers;
  std::vector<std::string> m_masterLayerOrder;
  std::string m_masterLayerId;   // the master layer being read
  std::stack<CDROutputElementList> m_masterOutputElementsStack;
  std::queue<CDROutputElementList> m_masterOutputElementsQueue;
  bool m_pageHasMasterLayers;   // the page listed master layers
  // drawing elements drawn again (master layers on each page, PowerClip contents): capped against
  // crafted files
  unsigned long m_copiedElements;
  // the master page's frame, each master layer was placed in (a page can have another size)
  std::map<std::string, CDRPage> m_masterLayerFrames;

  // PowerClips (CorelDRAW X6+): each one's contents in drawing order, by id, with the page frame
  // they were placed in (the clpt list comes before the pages)
  struct ClipContents
  {
    std::vector<CDROutputElementList> elements;
    CDRPage frame;
    ClipContents() : elements(), frame() {}
  };
  std::map<unsigned, ClipContents> m_clipContents;
  bool m_isInClipList;
  unsigned m_clipId;   // the contents being read
  unsigned m_currentPowerClip;   // the current object's contents (it is a PowerClip), by id

  CDRParserState &m_ps;
};

} // namespace libcdr

#endif /* __CDRCOLLECTOR_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
