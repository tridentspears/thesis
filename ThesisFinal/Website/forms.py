from django import forms
from .models import UserInfo,AddressInfo

class RegisterForm(forms.ModelForm):
    password = forms.CharField(widget=forms.PasswordInput)
    re_enter_pwd = forms.CharField(widget=forms.PasswordInput)

    class Meta:
        model = UserInfo
        fields = ['firstname', 'lastname', 'username', 'email' , 'password']

    def clean(self):
        cleaned_data = super().clean()
        password = cleaned_data.get('password')
        re_enter_pwd = cleaned_data.get('re_enter_pwd')

        if password != re_enter_pwd:
            self.add_error('re_enter_pwd', "Passwords do not match")

        return cleaned_data

    def save(self, commit=True):
        user = super(RegisterForm, self).save(commit=False)
        user.set_password(self.cleaned_data["password"])
        if commit:
            user.save()
            AddressInfo.objects.create(user=user)
        return user


class Loginform(forms.Form):
    username = forms.CharField(max_length=65)
    password = forms.CharField(max_length=65, widget=forms.PasswordInput)

class AddressInformation(forms.ModelForm):
    class Meta:
        model=AddressInfo
        fields = ['address','phone_number']