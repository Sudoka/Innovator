#pragma once

class Bindable {
public:
  virtual void bind() = 0;
  virtual void unbind() = 0;
};

class BindScope {
public:
  BindScope(Bindable * b) : bindable(b) {
    this->bindable->bind();
  }
  ~BindScope() {
    this->bindable->unbind();
  }
private:
  Bindable * bindable;
};
